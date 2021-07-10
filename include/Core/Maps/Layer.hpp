#ifndef CORE_MAPS_LAYER_HPP
#define CORE_MAPS_LAYER_HPP

#include <BLIB/Files/Binary.hpp>
#include <BLIB/Logging.hpp>

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
class Layer : public bl::file::binary::SerializableObject {
public:
    /**
     * @brief Construct an empty layer
     *
     */
    Layer();

    /**
     * @brief Copy constructs the layer
     *
     * @param copy The layer to copy
     */
    Layer(const Layer& copy);

    /**
     * @brief Move constructs the layer
     *
     * @param copy The layer to move from
     */
    Layer(Layer&& mlayer);

    /**
     * @brief Copies the given layer
     *
     * @param copy The layer to copy
     * @return Layer& A reference to this layer
     */
    Layer& operator=(const Layer& copy);

    /**
     * @brief Move assigns the layer
     *
     * @param mlayer The layer to move from
     * @return Layer& A reference to this layer
     */
    Layer& operator=(Layer&& mlayer);

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
    bl::file::binary::SerializableField<1, std::uint32_t> w;
    bl::file::binary::SerializableField<2, std::uint32_t> h;
    bl::file::binary::SerializableField<3, std::vector<T>> data;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
Layer<T>::Layer()
: w(*this)
, h(*this)
, data(*this) {}

template<typename T>
Layer<T>::Layer(const Layer& copy)
: Layer() {
    *this = copy;
}

template<typename T>
Layer<T>::Layer(Layer&& m)
: Layer() {
    *this = std::forward<Layer<T>>(m);
}

template<typename T>
Layer<T>& Layer<T>::operator=(const Layer& copy) {
    w    = copy.w.getValue();
    h    = copy.h.getValue();
    data = copy.data.getValue();
    return *this;
}

template<typename T>
Layer<T>& Layer<T>::operator=(Layer&& copy) {
    w               = copy.w.getValue();
    h               = copy.h.getValue();
    data.getValue() = copy.data.getMovable();
    return *this;
}

template<typename T>
void Layer<T>::create(unsigned int width, unsigned int height, const T& val) {
    w = width;
    h = height;
    data.getValue().clear();
    data.getValue().resize(width * height, val);
}

template<typename T>
unsigned int Layer<T>::width() const {
    return w.getValue();
}

template<typename T>
unsigned int Layer<T>::height() const {
    return h.getValue();
}

template<typename T>
const T& Layer<T>::get(unsigned int x, unsigned int y) const {
    const unsigned int i = y * w.getValue() + x;
    if (i < data.getValue().size()) { return data.getValue()[i]; }
    else {
        static const T null{};
        BL_LOG_WARN << "Out of bounds layer access: pos=(" << x << "," << y << ") size=("
                    << w.getValue() << "," << h.getValue() << ")";
        return null;
    }
}

template<typename T>
T& Layer<T>::getRef(unsigned int x, unsigned int y) {
    const unsigned int i = y * w.getValue() + x;
    if (i < data.getValue().size()) { return data.getValue()[i]; }
    else {
        static T null{};
        BL_LOG_WARN << "Out of bounds layer access: pos=(" << x << "," << y << ") size=("
                    << w.getValue() << "," << h.getValue() << ")";
        return null;
    }
}

template<typename T>
void Layer<T>::set(unsigned int x, unsigned int y, const T& value) {
    const unsigned int i = y * w.getValue() + x;
    if (i < data.getValue().size()) { data.getValue()[i] = value; }
}

} // namespace map
} // namespace core

#endif
