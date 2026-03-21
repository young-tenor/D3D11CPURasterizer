# D3D11 CPU Rasterizer
C++와 D3D11로 진행한 CPU 래스터라이저 프로젝트입니다. 엔진 프로그래밍보다는 그래픽스 알고리즘 구현에 집중했습니다. D3D11은 화면을 출력하는 용도로만 사용했습니다. 즉, 셰이더를 포함한 그래픽스 파이프라인을 C++로 구현했습니다.

월드 좌표계의 $x$ 범위는 $[-aspect, aspect]$, $y$ 범위는 $[-1, 1]$으로 가정했습니다.

아래처럼 main 함수에서 App 포인터에 할당하는 객체를 바꿔가며 그래픽스 알고리즘을 검사할 수 있습니다.

```cpp
const auto hello_triangle = new HelloTriangle();
const auto projection = new Projection();
const auto depth_buffering = new DepthBuffering();
const auto blinn_phong = new BlinnPhong();

App *app = blinn_phong;
```
### 목차
* [Hello Triangle](#hello-triangle)
* [Projection](#projection)
* [Depth Buffering](#debth-buffering)
* [Blinn-Phong](#blinn-phong)

## Hello Triangle
<img width="1282" height="752" alt="image" src="https://github.com/user-attachments/assets/ddab6710-da9c-4a96-818c-37c9b05d4a83" />

## Projection
<img width="1282" height="752" alt="image" src="https://github.com/user-attachments/assets/a1d12da1-f9b8-40ee-a9c0-63c0dd198c01" />
<img width="1282" height="752" alt="image" src="https://github.com/user-attachments/assets/236aac28-8b26-4a66-905e-dac0558c8e1b" />
<img width="1282" height="752" alt="image" src="https://github.com/user-attachments/assets/94df1496-2ec0-45ee-ba0b-bac329880c85" />

## Depth Buffering
<img width="1282" height="752" alt="image" src="https://github.com/user-attachments/assets/ac631661-f0aa-4eca-95b2-7559c1d3b90a" />
<img width="1282" height="752" alt="image" src="https://github.com/user-attachments/assets/60337301-668d-48c2-ac02-036c68cde524" />

## Blinn-Phong
<img width="1282" height="752" alt="image" src="https://github.com/user-attachments/assets/7c85a420-73d2-4fa2-a177-82ed4023417f" />
