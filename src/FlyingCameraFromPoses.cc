#include <gazebo/rendering/UserCamera.hh>
#include <gazebo/gui/GuiIface.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/common/Plugin.hh>
#include <ignition/math/Vector3.hh>
#include <ignition/math/Quaternion.hh>
#include <fstream>
#include <sstream>
#include <vector>

namespace gazebo
{
  struct CameraPose
  {
    ignition::math::Vector3d position;
    double yaw;
  };

  class FlyingCameraFromPoses : public SystemPlugin
  {
  private:
    rendering::UserCameraPtr camera;
    event::ConnectionPtr updateConnection;
    std::vector<CameraPose> waypoints;
    size_t currentIndex = 0;
    double t = 0.0;
    double segmentTime = 3.0;  // segundos para cada transição

  public:
    void Load(int /*_argc*/, char ** /*_argv*/) override
    {
      std::cout << "[FlyingCamera] Carregando poses...\n";
      LoadWaypoints("/tmp/camera_poses.txt");
      updateConnection = event::Events::ConnectPreRender(std::bind(&FlyingCameraFromPoses::OnUpdate, this));
    }

    void LoadWaypoints(const std::string &filename)
    {
      std::ifstream file(filename);
      std::string line;
      while (std::getline(file, line))
      {
        std::istringstream ss(line);
        double x, y, z, yaw;
        if (ss >> x >> y >> z >> yaw)
        {
          CameraPose pose;
          pose.position = ignition::math::Vector3d(x, y, z);
          pose.yaw = yaw;
          waypoints.push_back(pose);
        }
      }

      if (waypoints.size() < 2)
        std::cerr << "[FlyingCamera] ⚠️ Poucos pontos de pose carregados.\n";
      else
        std::cout << "[FlyingCamera] ✅ " << waypoints.size() << " poses carregadas.\n";
    }

    void OnUpdate()
    {
      if (!camera)
      {
        camera = gui::get_active_camera();
        if (!camera)
          return;
        std::cout << "[FlyingCamera] 🎥 Câmera detectada.\n";
        t = 0.0;
      }

      if (currentIndex + 1 >= waypoints.size())
        return;

      const CameraPose &start = waypoints[currentIndex];
      const CameraPose &end = waypoints[currentIndex + 1];

      double alpha = t / segmentTime;
      if (alpha > 1.0)
      {
        currentIndex++;
        t = 0.0;
        return;
      }

      // Interpolação linear de posição
      ignition::math::Vector3d pos = start.position + (end.position - start.position) * alpha;

      // Interpolação linear de yaw
      double yaw = start.yaw + (end.yaw - start.yaw) * alpha;
      ignition::math::Quaterniond rot(0, 0, yaw);

      camera->SetWorldPosition(pos);
      camera->SetWorldRotation(rot);

      t += 0.01;  // chamada a cada frame (~100 fps)
    }
  };

  GZ_REGISTER_SYSTEM_PLUGIN(FlyingCameraFromPoses)
}

