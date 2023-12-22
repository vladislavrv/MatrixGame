#include <keyboard.hpp>

#include <MatrixConfig.hpp>

#include <windows.h>

#include <bitset>

extern CMatrixConfig g_Config;

namespace Keyboard {

static std::bitset<256> down_keys;

void on_key_down(uint8_t vk)
{
    down_keys.set(vk);
}

void on_key_up(uint8_t vk)
{
    down_keys.reset(vk);
}

bool is_down(uint8_t vk)
{
    return down_keys.test(vk);
}

bool isVKeyPressed(uint32_t key)
{
    // return (GetAsyncKeyState(key) & 0x8000) == 0x8000;
    return is_down(key);
}

bool isKeyPressed(EKeyAction ka)
{
    return isVKeyPressed(g_Config.m_KeyActions[ka]);
}

} // namespace Keyboard