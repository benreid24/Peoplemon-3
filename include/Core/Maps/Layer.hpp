#ifndef CORE_MAPS_LAYER_HPP
#define CORE_MAPS_LAYER_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Serialization.hpp>

#include <cstdint>
#include <vector>

namespace core
{
namespace map
{
/**
 * @brief Generic map layer class. Can be used for any type of layer
 *
 * @tparam T The underlying type representing an object in the layer
 * @ingroup Maps
 */
template<typename T>
class Layer {
public:
    /**
     * @brief Construct an empty layer
     *
     */
    Layer();

    /**
     * @brief Clears all stored data (if any) and creates the layer with the given size
     *
     * @param width The width of the layer in tiles
     * @param height The height of the layer in tiles
     * @param val The default value to fill the layer with
     */
    void create(unsigned int width, unsigned int height, const T& val = T());

    /**
     * @brief Returns the width of the layer in tiles
     *
     */
    unsigned int width() const;

    /**
     * @brief Returns the height of the layer in tiles
     *
     */
    unsigned int height() const;

    /**
     * @brief Returns the object in the layer at the given position
     *
     * @param x The zero indexed x coordinate in tiles
     * @param y The zero indexed y coordinate in tiles
     * @return const T& The value at the given position
     */
    const T& get(unsigned int x, unsigned int y) const;

    /**
     * @brief Returns the object in the layer at the given position
     *
     * @param x The zero indexed x coordinate in tiles
     * @param y The zero indexed y coordinate in tiles
     * @return The value at the given position
     */
    T& getRef(unsigned int x, unsigned int y);

    /**
     * @brief Sets the object in the layer at the position to the given value
     *
     * @param x The zero indexed x coordinate in tiles
     * @param y The zero indexed y coordinate in tiles
     * @param value The value to set to
     */
    void set(unsigned int x, unsigned int y, const T& value);

private:
    std::uint32_t w;
    std::uint32_t h;
    std::vector<T> data;

    friend class bl::serial::SerializableObject<Layer<T>>;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
Layer<T>::Layer()
: w(0)
, h(0) {}

template<typename T>
void Layer<T>::create(unsigned int width, unsigned int height, const T& val) {
    w = width;
    h = height;
    data.clear();
    data.resize(width * height, val);
}

template<typename T>
unsigned int Layer<T>::width() const {
    return w;
}

template<typename T>
unsigned int Layer<T>::height() const {
    return h;
}

template<typename T>
const T& Layer<T>::get(unsigned int x, unsigned int y) const {
    const unsigned int i = y * w + x;
    if (i < data.size()) { return data[i]; }
    else {
        static const T null{};
        BL_LOG_WARN << "Out of bounds layer access: pos=(" << x << "," << y << ") size=(" << w
                    << "," << h << ")";
        return null;
    }
}

template<typename T>
T& Layer<T>::getRef(unsigned int x, unsigned int y) {
    const unsigned int i = y * w + x;
    if (i < data.size()) { return data[i]; }
    else {
        static T null{};
        BL_LOG_WARN << "Out of bounds layer access: pos=(" << x << "," << y << ") size=(" << w
                    << "," << h << ")";
        return null;
    }
}

template<typename T>
void Layer<T>::set(unsigned int x, unsigned int y, const T& value) {
    const unsigned int i = y * w + x;
    if (i < data.size()) { data[i] = value; }
}

} // namespace map
} // namespace core

namespace bl
{
namespace serial
{
template<typename T>
struct SerializableObject<core::map::Layer<T>> : public SerializableObjectBase {
    using Layer = core::map::Layer<T>;

    SerializableField<1, Layer, std::uint32_t> w;
    SerializableField<2, Layer, std::uint32_t> h;
    SerializableField<3, Layer, std::vector<T>> data;

    SerializableObject()
    : w("w", *this, &Layer::w, SerializableFieldBase::Required{})
    , h("h", *this, &Layer::h, SerializableFieldBase::Required{})
    , data("data", *this, &Layer::data, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
