## 光照模型与光线追踪

### 1. Phong光照模型
#### 1.1 实验结果
<table>
    <tr>
        <td><center>
            <img src="../out/a01.png" alt="a01" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a01n.png" alt="a01n" width="150"/>
        </td>
        <td><center>
            <img src="../out/a01d.png" alt="a01d" width="150"/>
        </center></td>
    </tr>
</table>

#### 1.2 实现方式

**获取光照强度**
通过计算交点和光源的距离，使用下面的公式计算交点的光照强度。
$$
I(x_{surf})=\frac{I}{\alpha d^2}
$$
其中，$I$ 是光源的光照强度，$\alpha$ 是光源的衰减系数，$d$ 是交点和光源的距离。

**漫反射着色器**
使用下面的公式计算漫反射的光照强度。
$$
I_{diffuse}=k_{diffuse}* I_{light} * clamp(L, N)
$$

$$
clamp(L, N)=max(0, L \cdot N)
$$

其中，$k_{diffuse}$ 是漫反射系数，$I_{light}$ 是光源的光照强度，$L$ 是光源的光照方向，$N$ 是交点的法向量。

**镜面反射着色器**
使用下面的公式计算镜面反射的光照强度。
$$
I_{specular}=k_{specular}* I_{light} * clamp(R, V)^{s}
$$
其中，$k_{specular}$ 是镜面反射系数，$I_{light}$ 是光源的光照强度，$R$ 是反射方向，$V$ 是视线方向，$s$ 是光泽度。

反射方向$R$ 是通过入射方向$E$ 和法向量$N$ 计算得到。
$$
R=E-2(N \cdot E)N
$$

**环境光着色器**
使用下面的公式计算环境光的光照强度。
$$
I_{ambient}=k_{diffuse}* I_{ambient}
$$
其中，$k_{diffuse}$ 是漫反射系数，$I_{ambient}$ 是光源的光照强度。

**Phong光照模型**
遍历所有的光源，将漫反射和镜面反射的光照强度相加，然后在把环境光的光照强度相加，得到最终的光照强度。
$$
I_{phong}=I_{ambient}+ \sum_{i \in lights} (I_{specular, i}+I_{diffuse, i})
$$

### 2. 光线投射
#### 2.1 平面
**实验结果**
<table>
    <tr>
        <td><center>
            <img src="../out/a01.png" alt="a01" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a01n.png" alt="a01n" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a01d.png" alt="a01d" width="150"/>
        </center></td>
    </tr>
</table>

**实现方式**
使用$P \cdot n=d$表示平面，$P$ 是平面上的点，$n$ 是平面的法向量，$d$ 是平面到原点的距离。使用$O+tD$表示光线，$O$ 是光线的起点，$D$ 是光线的方向，$t$ 是交点相对光线的起点距离。使用下面的公式计算交点。
$$
t=\frac{d-O \cdot n}{D \cdot n}
$$
然后判断交点是否在光源之后，即$t>t_{min}$。然后判断是否在光线已有交点之前。若满足上述条件，则更新交点。

#### 2.2 三角形
**实验结果**
<table>
    <tr>
        <td><center>
            <img src="../out/a02.png" alt="a02" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a02n.png" alt="a02n" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a02d.png" alt="a02d" width="150"/>
        </center></td>
    </tr>
</table>

**实现方式**
使用Möller–Trumbore算法计算三角形和光线的交点。首先用三角形三个顶点表示交点P。
$$
P=(1-u-v)A+uB+vC
$$
代入光线方程$O+tD$，得到
$$
O+tD=(1-u-v)A+uB+vC
$$
整理得到
$$
\begin{bmatrix}
    -D & (B - A) & (C - A) 
\end{bmatrix}
\begin{bmatrix}
    t \\
    u \\
    v \\
\end{bmatrix}
=O-A
$$
通过$u$, $v$, 得到$1-u-v$的值，判断三者是否都在$[0, 1]$之间。然后判断$t$是否在$[t_{min}, t_{exist}]$之间。若满足上述条件，则更新交点。

#### 2.3 变换类
**实验结果**
<table>
    <tr>
        <td><center>
            <img src="../out/a03.png" alt="a03" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a03n.png" alt="a03n" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a03d.png" alt="a03d" width="150"/>
        </center></td>
    </tr>
    <tr>
        <td><center>
            <img src="../out/a04.png" alt="a04" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a04n.png" alt="a04n" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a04d.png" alt="a04d" width="150"/>
        </center></td>
    </tr>
    <tr>
        <td><center>
            <img src="../out/a05.png" alt="a05" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a05n.png" alt="a05n" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a05d.png" alt="a05d" width="150"/>
        </center></td>
</table>


**实现方式**
存储一个变换矩阵$M$，及其逆变换矩阵$M^{-1}$，使用下面的公式将光源从世界坐标系变换到物体坐标系。
$$
O_{object}=M^{-1}[O_{world}, 1]
$$
$$
D_{object}=M^{-1}[D_{world}, 0]
$$

调用对象的intersect函数，判断交点。得到交点之后，将交点的法向量从局部坐标系变换到世界坐标系，使用下面的变换公式。
$$
N_{world}=(M^{-1})^TN_{object}
$$

### 3. 光线追踪与抗锯齿

#### 3.1 实验结果

<table>
    <tr>
        <td><center>
            <img src="../out/a06.png" alt="a06" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a06n.png" alt="a06n" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a06d.png" alt="a06d" width="150"/>
        </center></td>
    </tr>
    <tr>
        <td><center>
            <img src="../out/a07.png" alt="a07" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a07n.png" alt="a07n" width="150"/>
        </center></td>
        <td><center>
            <img src="../out/a07d.png" alt="a07d" width="150"/>
        </center></td>
    </tr>
</table>

#### 3.2 实现方式

当bounces大于0时，进行反射。使用下面的公式计算反射方向。

$$
R=E-2(N \cdot E)N
$$

然后递归调用trace函数，计算反射光线的颜色。使用下面的公式计算反射光线的颜色。
$$
I_{total} = I_{direct} + I_{indirect} * k_{specular}
$$

其中，$I_{total}$ 是总的光照强度，$I_{direct}$ 是直接光照强度，$I_{indirect}$ 是间接光照强度，$k_{specular}$ 是镜面反射系数。

要计算阴影的投射，将从这个点（Hit）向光源（light）发送光线。如果在光源之前有相交点（Hit），则当前的表面点处于阴影中，并忽略来自该光源的直接照明。

必须将阴影光线发送到所有光源。必须同等地将tmin设置为某个极小值，防止与自己相交。

为了实现抗锯齿，使用随机采样的方法。循环进行16次抖动采样，计算每个采样点的颜色，然后将所有采样点的颜色相加，最后除以采样点的数量。使用下面的公式计算抗锯齿。
$$
I_{antialiasing}=\frac{1}{N}\sum_{i=1}^{N}I_{i}
$$
其中，$I_{antialiasing}$ 是抗锯齿的光照强度，$N$ 是采样点的数量，$I_{i}$ 是第$i$个采样点的光照强度。

在此之后，再使用高斯滤波器进行模糊处理。下面是高斯滤波器的公式。

$$
G(x, y) = \frac{1}{2\pi\sigma^2}e^{-\frac{x^2+y^2}{2\sigma^2}}
$$

但我们这里是离散的情况，且固定kernel大小为3x3，所以我们使用下面的公式。

$$
G(x, y) = \frac{1}{16} \begin{bmatrix}
    1 & 2 & 1 \\
    2 & 4 & 2 \\
    1 & 2 & 1
\end{bmatrix}
$$

然后将每个像素点的颜色和周围8个像素点的颜色相乘，然后除以16，得到模糊处理后的颜色。使用下面的公式计算模糊处理后的颜色。
$$
I_{blur}=\frac{1}{16}\sum_{i=-1}^{1}\sum_{j=-1}^{1}I_{i,j}
$$
其中，$I_{blur}$ 是模糊处理后的颜色，$I_{i,j}$ 是第$i,j$个像素点的颜色。
最后将模糊处理后的颜色赋值给当前像素点的颜色。


