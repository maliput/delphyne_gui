// Copyright 2018 Toyota Research Institute

#include <delphyne/protobuf/world_stats.pb.h>

#include <ignition/common/Console.hh>
#include <ignition/common/PluginMacros.hh>
#include <ignition/common/Time.hh>

#include "TimeWidget.hh"

namespace delphyne
{
namespace gui
{
  class TimeWidgetPrivate
  {
    /// \brief Message holding latest world statistics
    public: ignition::msgs::WorldStatisticsD msg;

    /// \brief Service to send world control requests
    public: std::string controlService;

    /// \brief Mutex to protect msg
    public: std::recursive_mutex mutex;

    /// \brief Communication node
    public: ignition::transport::Node node;

    /// \brief The multi step value
    public: unsigned int multiStep = 1u;
  };
}
}

using namespace delphyne;
using namespace gui;

/////////////////////////////////////////////////
TimeWidget::TimeWidget()
  : Plugin(), dataPtr(new TimeWidgetPrivate)
{
}

/////////////////////////////////////////////////
TimeWidget::~TimeWidget()
{
}

/////////////////////////////////////////////////
void TimeWidget::LoadConfig(const tinyxml2::XMLElement *_pluginElem)
{
  // Default name in case user didn't define one
  if (this->title.empty())
    this->title = "Time panel";

  auto mainLayout = new QGridLayout();

  // Create elements from configuration
  if (_pluginElem)
  {
    // World control
    if (auto controlElem = _pluginElem->FirstChildElement("world_control"))
    {
      // For service requests
      if (auto serviceElem = controlElem->FirstChildElement("service"))
        this->dataPtr->controlService = serviceElem->GetText();

      if (this->dataPtr->controlService.empty())
      {
        ignerr << "Must specify a service for world control requests."
               << std::endl;
      }
      // Play / pause buttons
      else if (auto playElem = controlElem->FirstChildElement("play_pause"))
      {
        auto hasPlay = false;
        playElem->QueryBoolText(&hasPlay);

        if (hasPlay)
        {
          // Play button
          auto playButton = new QPushButton("Play");
          playButton->setObjectName("playButton");
          this->connect(playButton, SIGNAL(clicked()), this, SLOT(OnPlay()));
          this->connect(this, SIGNAL(Playing()), playButton, SLOT(hide()));
          this->connect(this, SIGNAL(Paused()), playButton, SLOT(show()));

          // Pause button
          auto pauseButton = new QPushButton("Pause");
          pauseButton->setObjectName("pauseButton");
          this->connect(pauseButton, SIGNAL(clicked()), this, SLOT(OnPause()));
          this->connect(this, SIGNAL(Playing()), pauseButton, SLOT(show()));
          this->connect(this, SIGNAL(Paused()), pauseButton, SLOT(hide()));

          auto stepButton = new QPushButton("Step");
          stepButton->setObjectName("stepButton");
          QLabel *stepLabel = new QLabel(tr("Steps:"));
          QSpinBox *stepSpinBox = new QSpinBox;
          stepSpinBox->setRange(1, 9999);

          this->connect(stepButton, SIGNAL(clicked()), this, SLOT(OnStep()));
          this->connect(this, &TimeWidget::Playing,
            [=]()
            {
              stepButton->setDisabled(true);
              stepSpinBox->setDisabled(true);
            });
          this->connect(this, &TimeWidget::Paused,
            [=]()
            {
              stepButton->setDisabled(false);
              stepSpinBox->setDisabled(false);
            });
          this->connect(stepSpinBox,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            [=](int _newValue) {this->dataPtr->multiStep = _newValue;});

          mainLayout->addWidget(playButton,  0, 0);
          mainLayout->addWidget(pauseButton, 0, 0);
          mainLayout->addWidget(stepButton,  0, 1);
          mainLayout->addWidget(stepLabel,   0, 2);
          mainLayout->addWidget(stepSpinBox, 0, 3);

          auto startPaused = false;
          if (auto pausedElem = controlElem->FirstChildElement("start_paused"))
          {
            pausedElem->QueryBoolText(&startPaused);
          }
          if (startPaused)
            this->Paused();
          else
            this->Playing();
        }
      }
    }

    // World stats
    if (auto statsElem = _pluginElem->FirstChildElement("world_stats"))
    {
      // Subscribe
      std::string topic;
      if (auto topicElem = statsElem->FirstChildElement("topic"))
        topic = topicElem->GetText();

      if (topic.empty())
      {
        ignerr << "Must specify a topic to subscribe to world statistics."
               << std::endl;
      }
      else
      {
        // Subscribe to world_stats
        if (!this->dataPtr->node.Subscribe(topic, &TimeWidget::OnWorldStatsMsg,
            this))
        {
          ignerr << "Failed to subscribe to [" << topic << "]" << std::endl;
        }
        else
        {
          // Sim time
          if (auto simTimeElem = statsElem->FirstChildElement("sim_time"))
          {
            auto hasSim = false;
            simTimeElem->QueryBoolText(&hasSim);

            if (hasSim)
            {
              auto simTime = new QLabel("N/A");
              simTime->setObjectName("simTimeLabel");
              this->connect(this, SIGNAL(SetSimTime(QString)), simTime,
                  SLOT(setText(QString)));

              mainLayout->addWidget(new QLabel("Sim time"), 1, 0);
              mainLayout->addWidget(simTime, 1, 1);
            }
          }

          // Real time
          if (auto realTimeElem = statsElem->FirstChildElement("real_time"))
          {
            auto hasReal = false;
            realTimeElem->QueryBoolText(&hasReal);

            if (hasReal)
            {
              auto realTime = new QLabel("N/A");
              realTime->setObjectName("realTimeLabel");
              this->connect(this, SIGNAL(SetRealTime(QString)), realTime,
                  SLOT(setText(QString)));

              mainLayout->addWidget(new QLabel("Real time"), 1, 2);
              mainLayout->addWidget(realTime, 1, 3);
            }
          }

          // Real time factor
          if (auto realTimeFactorElem =
                statsElem->FirstChildElement("real_time_factor"))
          {
            auto hasRTF = false;
            realTimeFactorElem->QueryBoolText(&hasRTF);

            if (hasRTF)
            {
              auto rtf = new QLabel("N/A");
              rtf->setObjectName("realTimeFactorLabel");
              this->connect(this, SIGNAL(SetRealTimeFactor(QString)), rtf,
                  SLOT(setText(QString)));

              mainLayout->addWidget(new QLabel("Real Time Factor"), 1, 4);
              mainLayout->addWidget(rtf, 1, 5);
            }
          }
        }
      }
    }
  }

  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  this->setLayout(mainLayout);
}

/////////////////////////////////////////////////
void TimeWidget::ProcessMsg()
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  ignition::common::Time time;

  if (this->dataPtr->msg.has_sim_time())
  {
    time.sec = this->dataPtr->msg.sim_time().sec();
    time.nsec = this->dataPtr->msg.sim_time().nsec();

    this->SetSimTime(QString::fromStdString(time.FormattedString()));
  }

  if (this->dataPtr->msg.has_real_time())
  {
    time.sec = this->dataPtr->msg.real_time().sec();
    time.nsec = this->dataPtr->msg.real_time().nsec();

    this->SetRealTime(QString::fromStdString(time.FormattedString()));
  }

  if (this->dataPtr->msg.has_real_time_factor())
  {
    double rtf = this->dataPtr->msg.real_time_factor();

    this->SetRealTimeFactor(QString("%1").arg(rtf, 7, 'f', 2, '0'));
  }

  if (this->dataPtr->msg.has_paused())
  {
    if (this->dataPtr->msg.paused())
      this->Paused();
    else
      this->Playing();
  }
}

/////////////////////////////////////////////////
void TimeWidget::OnWorldStatsMsg(const ignition::msgs::WorldStatisticsD &_msg)
{
  std::lock_guard<std::recursive_mutex> lock(this->dataPtr->mutex);

  this->dataPtr->msg.CopyFrom(_msg);
  QMetaObject::invokeMethod(this, "ProcessMsg");
}

/////////////////////////////////////////////////
void TimeWidget::OnPlay()
{
  std::function<void(const ignition::msgs::Boolean &, const bool)> cb =
      [this](const ignition::msgs::Boolean &/*_rep*/, const bool _result)
  {
    if (_result)
      QMetaObject::invokeMethod(this, "Playing");
  };

  ignition::msgs::WorldControl req;
  req.set_pause(false);
  this->dataPtr->node.Request(this->dataPtr->controlService, req, cb);
}

/////////////////////////////////////////////////
void TimeWidget::OnPause()
{
  std::function<void(const ignition::msgs::Boolean &, const bool)> cb =
      [this](const ignition::msgs::Boolean &/*_rep*/, const bool _result)
  {
    if (_result)
      QMetaObject::invokeMethod(this, "Paused");
  };

  ignition::msgs::WorldControl req;
  req.set_pause(true);
  this->dataPtr->node.Request(this->dataPtr->controlService, req, cb);
}

/////////////////////////////////////////////////
void TimeWidget::OnStep()
{
  std::function<void(const ignition::msgs::Boolean &, const bool)> cb =
      [this](const ignition::msgs::Boolean &/*_rep*/, const bool /*_result*/)
  {
  };

  ignition::msgs::WorldControl req;
  req.set_multi_step(this->dataPtr->multiStep);
  this->dataPtr->node.Request(this->dataPtr->controlService, req, cb);
}

// Register this plugin
IGN_COMMON_REGISTER_SINGLE_PLUGIN(delphyne::gui::TimeWidget,
                                  ignition::gui::Plugin)
