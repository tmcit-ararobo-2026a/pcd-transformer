# PCD Transformer

Point Cloud Data（`.pcd`）ファイルを読み込み、指定した座標変換（TF: 平行移動および回転）を適用して新たな `.pcd` ファイルとして出力するC++ツールです。

ROS 2 の `static_transform_publisher` などで設定されたパラメータ（$x, y, z$ および Roll, Pitch, Yaw）をそのまま適用して、センサ座標系（`livox_frame` など）からロボット基準座標系（`base_link` など）への点群座標変換を簡単に行えます。

---

## 特徴

* **柔軟な入力インターフェース**:
* コマンドライン引数による一括指定
* 対話型プロンプト（引数なしで実行した場合）


* **ROS互換の回転処理**: Roll $\rightarrow$ Pitch $\rightarrow$ Yaw（Z-Y-X順）の回転行列生成に対応
* **属性（Intensityなど）の保持**: 反射強度情報（`pcl::PointXYZI`）を含んだPCD変換に対応

---

## 依存ライブラリ

* **C++14** 以降
* **CMake** (3.10 以上)
* **PCL (Point Cloud Library)** 1.7 以上
* **Eigen3**

### Ubuntu での必要パッケージ取得

```bash
sudo apt update
sudo apt install build-essential cmake libpcl-dev

```

---

## ビルド方法

```bash
# リポジトリのクローン・ディレクトリ移動
cd ~/ws/pcd_transformer

# ビルド用ディレクトリの作成・移動
mkdir build && cd build

# コンパイル
cmake ..
make

```

---

## 使い方

### 1. コマンドライン引数で実行（推奨）

#### 基本書式

```bash
./pcd_transformer <入力PCD> <出力PCD> [x y z] [roll pitch yaw]

```

> ※ `x, y, z` はメートル [m]、`roll, pitch, yaw` はラジアン [rad] で指定します。

#### 実行例: `livox_frame` $\rightarrow$ `base_link` 変換

```bash
# ROS2: ros2 run tf2_ros static_transform_publisher --x 0.2 --y -0.25 --z 1.09 --yaw 0.0 --pitch -0.273 --roll 3.13 --frame-id base_link --child-frame-id livox_frame
./pcd_transformer input.pcd output.pcd 0.2 -0.25 1.09 3.13 -0.273 0.0

```

---

### 2. 対話形式で実行

引数を指定せずに実行すると、対話プロンプトが表示されます。

```bash
./pcd_transformer

```

#### 実行プロンプト例

```text
=========================================
          PCD Transformer (汎用版)        
=========================================
入力PCDファイルパス: input.pcd
出力PCDファイルパス: output.pcd

--- TFパラメータの入力 (未入力はデフォルト値) ---
x [m] [0]: 0.2
y [m] [0]: -0.25
z [m] [0]: 1.09
roll  [rad] [0]: 3.13
pitch [rad] [0]: -0.273
yaw   [rad] [0]: 0

```

---

## 出力PCDの確認方法 (プレビュー)

変換後の `.pcd` ファイルは `pcl_viewer` などで確認できます。

```bash
# pcl-tools のインストール (未導入の場合)
sudo apt install pcl-tools

# プレビュー表示
pcl_viewer output.pcd

```
