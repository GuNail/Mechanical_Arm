#ifndef CALCULATEANGLES_H
#define CALCULATEANGLES_H
#include <QList>

QList<int> Inverse_Kinematics(double X, double Y, double Z);
QList<int> forward_kinematics(double j1, double j2, double j3, double j4);
#endif // CALCULATEANGLES_H
