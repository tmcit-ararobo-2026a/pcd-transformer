#include <iostream>
#include <string>
#include <cmath>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/common/transforms.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " <input_livox_cloud.pcd> <output_base_cloud.pcd>" << std::endl;
        return -1;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);

    if (pcl::io::loadPCDFile<pcl::PointXYZ>(input_file, *cloud) == -1)
    {
        PCL_ERROR("Couldn't read file %s \n", input_file.c_str());
        return -1;
    }
    std::cout << "Loaded " << cloud->width * cloud->height << " points from " << input_file << std::endl;

    // ---------------------------------------------------------
    // livox_frame -> base_link への TF パラメータ設定
    // ---------------------------------------------------------
    // 平行移動 (m)
    float x = 0.2f;
    float y = -0.25f;
    float z = 1.09f;

    // 回転 (rad)
    float roll  = 3.13f;      // X軸まわり
    float pitch = -0.273f;    // Y軸まわり
    float yaw   = 0.0f;       // Z軸まわり

    // Eigenでの変換行列の生成 (ROSのZ-Y-X順回転)
    Eigen::Affine3f transform = Eigen::Affine3f::Identity();

    // 1. 平行移動を適用
    transform.translation() << x, y, z;

    // 2. 回転（Yaw -> Pitch -> Roll 順で適用）を適用
    transform.rotate(Eigen::AngleAxisf(yaw,   Eigen::Vector3f::UnitZ())
                   * Eigen::AngleAxisf(pitch, Eigen::Vector3f::UnitY())
                   * Eigen::AngleAxisf(roll,  Eigen::Vector3f::UnitX()));

    // 行列を出力して確認
    std::cout << "Transformation matrix (livox_frame -> base_link):\n" 
              << transform.matrix() << std::endl;

    // ---------------------------------------------------------
    // 座標変換の実行
    // ---------------------------------------------------------
    pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::transformPointCloud(*cloud, *transformed_cloud, transform);

    // ---------------------------------------------------------
    // PCDファイルの保存
    // ---------------------------------------------------------
    if (pcl::io::savePCDFileBinary(output_file, *transformed_cloud) == -1)
    {
        PCL_ERROR("Couldn't write file %s \n", output_file.c_str());
        return -1;
    }
    std::cout << "Saved converted cloud to " << output_file << std::endl;

    return 0;
}