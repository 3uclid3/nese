#pragma once

#include <memory>
#include <vector>

#include <nesesan/view/view.hpp>

namespace nese::san {

class view_registry
{
public:
    template<typename ViewT, typename... ArgsT>
    ViewT& add(ArgsT&&... args);

    template<typename FunctorT>
    void for_each(const FunctorT& functor) const;


private:
    std::vector<std::unique_ptr<view>> _views;
};

template<typename ViewT, typename... ArgsT>
ViewT& view_registry::add(ArgsT&&... args)
{
    _views.emplace_back(std::make_unique<ViewT>(std::forward<ArgsT>(args)...));

    return static_cast<ViewT&>(*_views.back());
}

template<typename FunctorT>
void view_registry::for_each(const FunctorT& functor) const
{
    for (const auto& view : _views)
    {
        functor(*view);
    }
}

} // namespace nese::san