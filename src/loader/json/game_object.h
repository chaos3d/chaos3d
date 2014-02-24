#include "loader/json/json_loader.h"
#include "go/game_object.h"
#include "go/component.h"
#include "io/memory_stream.h"
#include <rapidjson/document.h> // this seems to be not avoidable in this case

#if 0
template<>
game_object::ptr game_object::load_object_from<>(json_loader const&) {
    game_object* go = new game_object(nullptr);
    return ptr(go);
}
#endif

template<typename... Cms>
struct game_object::loader_constructor<json_loader, Cms...> {
    template<typename C, typename... CCms>
    void construct_component(json_loader const& loader, char const* tag, game_object* go) const{
        if (strcmp(C::tag, tag) == 0) {
            go->add_component<C>(loader);
        } else {
            return construct_component<CCms...>(loader, tag, go);
        }
    }
    
    // recursive termination
    template<typename... CCms>
    typename std::enable_if<sizeof...(CCms) == 0>::type
    construct_component(json_loader const&, char const*, game_object*) const{
        // TODO: report error
    }
    
    ptr operator() (json_loader const& loader) const {
        using rapidjson::Document;
        typedef rapidjson::GenericValue<Document::EncodingType, Document::AllocatorType> Value;
        Value const& value = loader.internal<Value>();
        
        ptr go(new game_object());
        
        auto& com = value["components"];
        if (com.IsObject()) {
            for (auto it = com.MemberBegin(); it != com.MemberEnd(); ++it) {
                auto* element = &it->value;
                construct_component<Cms...>(json_loader((void*)element), it->name.GetString(), go.get());
            }
        }
        return go;
    }
};