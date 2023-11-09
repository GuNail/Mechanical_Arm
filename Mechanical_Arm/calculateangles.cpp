#include "calculateangles.h"
#include <QDebug>
#include <math.h>

QList<int> Inverse_Kinematics(double X, double Y, double Z)
{

    float a1, a2, a3, a4; // a1为底部圆台高度 剩下三个为三个机械臂长度
    float j1, j4, j2, j3; // 四个姿态角
    float L, H, P;        // L =a2*sin(j2) + a3*sin(j2 + j3);H = a2*cos(j2) + a3*cos(j2 +
                          // j3); P为底部圆盘半径R
    float j_all;          // j2,j3,j4之和
    float len, high;      // 总长度,总高度
    float Cosj3, Sinj3;   // 用来存储cosj3,sinj3数值
    float Cosj2, Sinj2;
    float K1, K2;
    float n, m;
    n = 0;
    m = 0;

    P = 18;    // 底部圆盘半径
    a1 = 10.5; // 底部圆盘高度
    a2 = 10.5; // 机械臂长度
    a3 = 7.5;
    a4 = 20;

    if (X == 0)
        j1 = 90;
    else
        j1 = atan((Y + P) / X) * (57.3);

    for (int i = 0; i <= 180; i++)
    {
        j_all = 3.1415927 * i / 180;

        len = sqrt((Y + P) * (Y + P) + X * X);
        high = Z;

        L = len - a4 * sin(j_all);
        H = high - a4 * cos(j_all) - a1;

        Cosj3 = ((L * L) + (H * H) - ((a2) * (a2)) - ((a3) * (a3))) / (2 * (a2) * (a3));
        Sinj3 = (sqrt(1 - (Cosj3) * (Cosj3)));

        j3 = atan((Sinj3) / (Cosj3)) * (57.3);

        K2 = a3 * sin(j3 / 57.3);
        K1 = a2 + a3 * cos(j3 / 57.3);

        Cosj2 = (K2 * L + K1 * H) / (K1 * K1 + K2 * K2);
        Sinj2 = (sqrt(1 - (Cosj2) * (Cosj2)));

        j2 = atan((Sinj2) / (Cosj2)) * 57.3;
        j4 = j_all * 57.3 - j2 - j3;

        if (j2 >= 0 && j3 >= 0 && j4 >= -90 && j2 <= 180 && j3 <= 180 && j4 <= 90)
        {
            n = n + 1;
        }
    }

    for (int i = 0; i <= 180; i++)
    {
        j_all = 3.1415927 * i / 180;

        len = sqrt((Y + P) * (Y + P) + X * X);
        high = Z;

        L = len - a4 * sin(j_all);
        H = high - a4 * cos(j_all) - a1;

        Cosj3 = ((L * L) + (H * H) - ((a2) * (a2)) - ((a3) * (a3))) / (2 * (a2) * (a3));
        Sinj3 = (sqrt(1 - (Cosj3) * (Cosj3)));

        j3 = atan((Sinj3) / (Cosj3)) * (57.3);

        K2 = a3 * sin(j3 / 57.3);
        K1 = a2 + a3 * cos(j3 / 57.3);

        Cosj2 = (K2 * L + K1 * H) / (K1 * K1 + K2 * K2);
        Sinj2 = (sqrt(1 - (Cosj2) * (Cosj2)));

        j2 = atan((Sinj2) / (Cosj2)) * 57.3;
        j4 = j_all * 57.3 - j2 - j3;

        if (j2 >= 0 && j3 >= 0 && j4 >= -90 && j2 <= 180 && j3 <= 180 && j4 <= 90)
        {
            m = m + 1;
            if (m == n / 2 || m == (n + 1) / 2)
            {
                break;
            }
        }
    }

    if (signbit(j1))
    {
        j1 += 180;
    }
    QList<int> angle = {qRound(j1), qRound(j2) + 90, qRound(j3) + 90, qRound(j4) + 90};
    return angle;
}

QList<int> forward_kinematics(double j1, double j2, double j3, double j4)
{
    float a1, a2, a3, a4, P;

    a1 = 10.5;
    a2 = 10.5;
    a3 = 7.5;
    a4 = 20;
    P = 15;

    j1 /= 57.3;
    j2 /= 57.3;
    j3 /= 57.3;
    j4 /= 57.3;

    double X, Y, Z;

    Y = a2 * sin(j2) + a3 * sin(j2 + j3) + a4 * sin(j2 + j3 + j4) - P;
    Z = a1 + a2 * cos(j2) + a3 * cos(j2 + j3) + a4 * cos(j2 + j3 + j4);
    X = 1 / (tan(j1) / (Y + P));

    QList<int> coordinate = {qRound(X), qRound(Y), qRound(Z)};
    return coordinate;
}
