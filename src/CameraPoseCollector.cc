#include <gazebo/common/Plugin.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/gui/GuiIface.hh>
#include <gazebo/rendering/UserCamera.hh>
#include <chrono>
#include <fstream>
#include <thread>

namespace gazebo
{
  class CameraPoseLogger : public SystemPlugin
  {
  private:
    rendering::UserCameraPtr camera;
    event::ConnectionPtr updateConnection;
    std::ofstream file;
    double lastLogTime = 0;

  public:
    void Load(int /*_argc*/, char ** /*_argv*/) override
    {
      std::cout << "[PoseLogger] Plugin carregado. Aguardando câmera...\n";
      file.open("/tmp/camera_poses.txt", std::ios::out | std::ios::app);
      updateConnection = event::Events::ConnectPreRender(std::bind(&CameraPoseLogger::OnUpdate, this));
    }

    void OnUpdate()
    {
      if (!camera)
      {
        camera = gui::get_active_camera();
        if (!camera)
          return;
        std::cout << "[PoseLogger] 🎥 Câmera detectada. Iniciando coleta automática.\n";
      }

      static double t = 0.0;
      t += 0.01;

      if (t - lastLogTime >= 2.0)  // salva a cada 2 segundos
      {
        auto pose = camera->WorldPose();
        file << pose.Pos().X() << " "
             << pose.Pos().Y() << " "
             << pose.Pos().Z() << " "
             << pose.Rot().Euler().Z() << std::endl;
        std::cout << "[PoseLogger] Pose salva!\n";
        lastLogTime = t;
      }
    }

    ~CameraPoseLogger()
    {
      if (file.is_open())
        file.close();
    }
  };

  GZ_REGISTER_SYSTEM_PLUGIN(CameraPoseLogger)
}

