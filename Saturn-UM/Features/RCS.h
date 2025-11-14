#pragma once
#include "Aimbot.h"

struct WeaponRCSConfig { float yaw; float pitch; };
inline WeaponRCSConfig GetWeaponRCSConfig(const std::string& weaponName) {
    if (weaponName == "ak47")   return { 1.00f, 1.00f };
    if (weaponName == "m4a4")   return { 1.00f, 1.00f };
    if (weaponName == "m4a1")   return { 1.00f, 1.00f };
    if (weaponName == "famas")  return { 1.00f, 1.00f };
    if (weaponName == "galilar")return { 1.00f, 1.00f };
    if (weaponName == "aug")    return { 1.00f, 1.00f };
    if (weaponName == "sg556")  return { 1.00f, 1.00f };
    return { 1.00f, 1.00f };
}

namespace RCS
{
    inline int RCSBullet = 1;
    inline Vec2 RCSScale = { 1.4f, 1.4f };
    void UpdateAngles(const CEntity&, Vec2&);
    void RecoilControl(const CEntity& LocalPlayer);
}
