#include <gazebo/rendering/UserCamera.hh>
#include <gazebo/gui/GuiIface.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/Events.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/math/Quaternion.hh>
#include <gazebo/gazebo.hh>

namespace gazebo
{
  class MinimalCameraPlugin
  {
  private:
    rendering::UserCameraPtr camera;
    event::ConnectionPtr updateConnection;
    double t = 0.0;

  public:
    void Init()
    {
      updateConnection = event::Events::ConnectPreRender(
        std::bind(&MinimalCameraPlugin::OnUpdate, this));
    }

	void OnUpdate()
	{
	  if (!camera)
	  {
	    camera = gui::get_active_camera();
	    if (!camera)
	    {
	      std::cout << "[Plugin] Aguardando câmera...\n";
	      return;
	    }
	    std::cout << "[Plugin] 🎥 Câmera encontrada!\n";
	  }

	  // Trajetória circular
	  double r = 10.0;
	  double z = 5.0;
	  double speed = 0.001;
	  t += speed;

	  double x = r * cos(t);
	  double y = r * sin(t);

	  ignition::math::Vector3d pos(x, y, z);
	  ignition::math::Vector3d target(0, 0, 0);
	  ignition::math::Vector3d dir = (target - pos).Normalize();

	  ignition::math::Quaterniond rot;
	  rot.From2Axes(ignition::math::Vector3d::UnitX, dir);

	  camera->SetWorldPosition(pos);
	  camera->SetWorldRotation(rot);

	  std::cout << "[Plugin] Câmera movida para: " << pos << "\n";
	}

  };

  class PluginLoader : public SystemPlugin
  {
  public:
    void Load(int /*_argc*/, char ** /*_argv*/) override
    {
      static MinimalCameraPlugin plugin;
      plugin.Init();
    }
  };

  GZ_REGISTER_SYSTEM_PLUGIN(PluginLoader)
}

