#ifndef NEFORCE_TUI_DOM_LINEAR_GRADIENT_HPP__
#define NEFORCE_TUI_DOM_LINEAR_GRADIENT_HPP__

/**
 * @file linear_gradient.hpp
 * @brief 线性渐变
 *
 * 提供线性渐变色配置，用于 text/bg 的颜色渐变渲染。
 */

#include "NeForce/core/container/vector.hpp"
#include "NeForce/core/utility/color.hpp"
#include "NeForce/core/utility/optional.hpp"
NEFORCE_BEGIN_NAMESPACE__
NEFORCE_BEGIN_TUI__

/**
 * @addtogroup TUI TUI
 * @{
 */

/**
 * @struct linear_gradient
 * @brief 线性渐变配置
 *
 * 定义一组色标，颜色在色标之间线性插值。
 */
struct linear_gradient {
private:
    /**
     * @brief 色标
     */
    struct color_stop {
        _NEFORCE color color;     ///< 色标颜色
        optional<float> position; ///< 位置 (0.0-1.0)，none 表示自动均匀分布

        /**
         * @brief 构造色标
         * @param c 颜色
         * @param pos 位置
         */
        explicit color_stop(_NEFORCE color c, optional<float> pos = none) :
        color(move(c)),
        position(move(pos)) {}
    };

    float angle_ = 0.0F;       ///< 渐变角度
    vector<color_stop> stops_; ///< 色标列表

public:
    /**
     * @brief 默认构造
     */
    linear_gradient() {
        stops_.push_back(color_stop(_NEFORCE color::transparent(), 0.0F));
        stops_.push_back(color_stop(_NEFORCE color::black(), 1.0F));
    }

    /**
     * @brief 从起止颜色构造
     * @param begin 起始颜色
     * @param end 结束颜色
     */
    linear_gradient(_NEFORCE color begin, struct color end) {
        stops_.push_back(color_stop(begin, 0.0F));
        stops_.push_back(color_stop(end, 1.0F));
    }

    /**
     * @brief 设置渐变角度
     * @param a 角度
     * @return 自身引用
     */
    linear_gradient& angle(const float a) {
        angle_ = a;
        return *this;
    }

    /**
     * @brief 添加色标
     * @param c 颜色
     * @param pos 位置（none 表示跟随前一个色标的位置）
     * @return 自身引用
     */
    linear_gradient& add_stop(const struct color& c, optional<float> pos = none) {
        if (!pos.has_value()) {
            stops_.push_back(color_stop(c, none));
            return *this;
        }

        auto where = stops_.begin();
        while (where != stops_.end()) {
            const optional<float>& existing = where->position;
            if (existing.has_value() && existing.value() > pos.value()) {
                break;
            }
            ++where;
        }
        stops_.emplace(where, c, _NEFORCE move(pos));
        return *this;
    }

    /**
     * @brief 在指定位置采样颜色
     * @param t 归一化位置 (0.0-1.0)
     * @return 插值颜色
     */
    NEFORCE_NODISCARD _NEFORCE color sample(float t) const {
        if (stops_.empty()) {
            return _NEFORCE color{};
        }
        if (stops_.size() == 1) {
            return stops_[0].color;
        }

        const float position = (t < 0.0F) ? 0.0F : ((t > 1.0F) ? 1.0F : t);
        float p1 = stops_[0].position.value_or(0.0F);

        for (size_t i = 0; i + 1 < stops_.size(); ++i) {
            const float p2 = stops_[i + 1].position.value_or(p1);
            if (position < p2) {
                if (p2 <= p1) {
                    return stops_[i + 1].color;
                }
                return color::lerp(stops_[i].color, stops_[i + 1].color, (position - p1) / (p2 - p1));
            }
            p1 = p2;
        }

        return stops_[stops_.size() - 1].color;
    }
};

/** @} */ // TUI

NEFORCE_END_TUI__
NEFORCE_END_NAMESPACE__
#endif // NEFORCE_TUI_DOM_LINEAR_GRADIENT_HPP__
