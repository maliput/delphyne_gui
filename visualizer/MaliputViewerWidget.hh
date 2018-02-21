// Copyright 2017 Toyota Research Institute

#ifndef DELPHYNE_GUI_MALIPUTVIEWERWIDGET_HH
#define DELPHYNE_GUI_MALIPUTVIEWERWIDGET_HH

#include <memory>
#include <string>

#include <drake/automotive/maliput/api/road_geometry.h>

#include <ignition/gui/Plugin.hh>

#include "LayerSelectionWidget.hh"
#include "MaliputMesh.hh"
#include "MaliputViewerModel.hh"
#include "RenderMaliputWidget.hh"

namespace delphyne {
namespace gui {

/// \brief Plugin class that acts as a controller for the application.
///
/// Holds different view widgets and connects the model with the events and
/// changes in the GUI.
class MaliputViewerWidget : public ignition::gui::Plugin {
  Q_OBJECT

 public:
  /// \brief Default constructor.
  explicit MaliputViewerWidget(QWidget* parent = 0);

  /// \brief Default Destructor.
  virtual ~MaliputViewerWidget() = default;

 protected:
  /// \brief Override paintEngine to stop Qt From trying to draw on top of
  /// render window.
  /// \return NULL.
  virtual QPaintEngine* paintEngine() const;

  /// \brief Updates the model based on the @p key mesh name and @p newValue and
  /// the mesh on the GUI.
  /// \param[in] key Name of the mesh.
  /// \param[in] newValue Mesh visualization status.
  protected slots: void OnLayerMeshChanged(const std::string& key,
    MaliputMesh::State newValue);

 protected:
  /// \brief Overridden method to receive Qt paint event.
  /// \param[in] _e The event that happened.
  virtual void paintEvent(QPaintEvent* _e);

 private:
  /// \brief Builds the widgets of the GUI.
  void BuildGUI();

  /// \brief Widget to hold and modify the visualization status of each layer.
  LayerSelectionWidget* layerSelectionWidget{nullptr};

  /// \brief World render widget.
  RenderMaliputWidget* renderWidget{nullptr};

  /// \brief Model that holds the meshes and the visualization status.
  std::unique_ptr<MaliputViewerModel> model{};
};

}
}

#endif
