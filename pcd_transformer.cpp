#include <iostream>
#include <string>
#include <cmath>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/common/transforms.h>
#include <pcl/point_cloud.h>
#include <pcl/conversions.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>

// 入力ヘルパー関数
float get_input_float(const std::string& prompt, float default_val)
{
    std::cout << prompt << " [" << default_val << "]: ";
    std::string input;
    std::getline(std::cin, input);
    if (input.empty()) {
        return default_val;
    }
    try {
        return std::stof(input);
    } catch (...) {
        std::cout << "無効な入力です。デフォルト値 (" << default_val << ") を使用します。\n";
        return default_val;
    }
}

int main(int argc, char** argv)
{
    std::string input_file;
    std::string output_file;
    float x = 0.0f, y = 0.0f, z = 0.0f;
    float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;

    // ---------------------------------------------------------
    // 1. 引数の解析または対話入力
    // ---------------------------------------------------------
    if (argc >= 3)
    {
        input_file = argv[1];
        output_file = argv[2];

        if (argc >= 6) {
            x = std::stof(argv[3]);
            y = std::stof(argv[4]);
            z = std::stof(argv[5]);
        }
        if (argc >= 9) {
            roll  = std::stof(argv[6]);
            pitch = std::stof(argv[7]);
            yaw   = std::stof(argv[8]);
        }
    }
    else
    {
        std::cout << "=========================================\n";
        std::cout << "          PCD Transformer (汎用版)        \n";
        std::cout << "=========================================\n";

        std::cout << "入力PCDファイルパス: ";
        std::cin >> input_file;
        std::cout << "出力PCDファイルパス: ";
        std::cin >> output_file;
        std::cin.ignore(); // 残った改行コードをクリア

        std::cout << "\n--- TFパラメータの入力 (未入力はデフォルト値) ---\n";
        x = get_input_float("x [m]", 0.0f);
        y = get_input_float("y [m]", 0.0f);
        z = get_input_float("z [m]", 0.0f);

        roll  = get_input_float("roll  [rad]", 0.0f);
        pitch = get_input_float("pitch [rad]", 0.0f);
        yaw   = get_input_float("yaw   [rad]", 0.0f);
    }

    // ---------------------------------------------------------
    // 2. PCDファイルの読み込み (IntensityやRGBを保持する汎用読み込み)
    // ---------------------------------------------------------
    pcl::PCLPointCloud2 cloud2;
    if (pcl::io::loadPCDFile(input_file, cloud2) == -1)
    {
        PCL_ERROR("Error: ファイル %s を読み込めませんでした。\n", input_file.c_str());
        return -1;
    }
    
    // 座標変換用に PointXYZI または PointXYZ に変換 (全属性を維持)
    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);
    pcl::fromPCLPointCloud2(cloud2, *cloud);

    std::cout << "\nLoaded " << cloud->width * cloud->height << " points from " << input_file << std::endl;

    // ---------------------------------------------------------
    // 3. TF（変換行列）の作成
    // ---------------------------------------------------------
    Eigen::Affine3f transform = Eigen::Affine3f::Identity();

    // 平行移動
    transform.translation() << x, y, z;

    // 回転 (ROS標準: Yaw -> Pitch -> Roll 順)
    transform.rotate(Eigen::AngleAxisf(yaw,   Eigen::Vector3f::UnitZ())
                   * Eigen::AngleAxisf(pitch, Eigen::Vector3f::UnitY())
                   * Eigen::AngleAxisf(roll,  Eigen::Vector3f::UnitX()));

    std::cout << "\n使用する変換行列 (Transformation Matrix):\n" 
              << transform.matrix() << std::endl;

    // ---------------------------------------------------------
    // 4. 点群の座標変換
    // ---------------------------------------------------------
    pcl::PointCloud<pcl::PointXYZI>::Ptr transformed_cloud(new pcl::PointCloud<pcl::PointXYZI>);
    pcl::transformPointCloud(*cloud, *transformed_cloud, transform);

    // ---------------------------------------------------------
    // 5. PCDファイルの保存
    // ---------------------------------------------------------
    if (pcl::io::savePCDFileBinary(output_file, *transformed_cloud) == -1)
    {
        PCL_ERROR("Error: ファイル %s に書き込めませんでした。\n", output_file.c_str());
        return -1;
    }
    std::cout << "\nSuccessfully saved converted cloud to: " << output_file << std::endl;

    return 0;
}