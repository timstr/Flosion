#pragma once

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace flui {

    class ArgumentParser {
    public:

        template<typename T>
        void add(T& target);

        template<typename T>
        void add(T& target, std::initializer_list<T> permittedValues);

        template<typename T>
        void add(std::optional<T>& target);

        template<typename T>
        void add(std::optional<T>& target, std::initializer_list<T> permittedValues);

        // TODO: custom parsing functions e.g.
        //     void add(std::function<bool(const ui::String&)>);

        // TODO: allow variadic args e.g.
        //     template<typename T>
        //     void addVariadic(std::vector<T>&);
        // For simplicity, such a variadic list should only ever occur at the end

        bool parse(const std::vector<std::string>&);

    private:

        class Item {
        public:
            virtual ~Item() noexcept = default;

            // Positive: parse success
            // Zero:     failed to parse optional argument, safe to continue
            // Negative: failed to parse required argument, abort
            virtual int parse(const std::string&) = 0;
        };

        template<typename T>
        class ConcreteItem : public Item {
        public:
            ConcreteItem(T& target) : m_target(target) {
            
            }

        private:
            int parse(const std::string& s) override {
                std::stringstream ss;
                ss.str(s);
                return (ss >> m_target) ? 1 : -1;
            }

            T& m_target;
        };

        template<typename T>
        class ConcreteItem<std::optional<T>> : public Item {
        public:
            ConcreteItem(std::optional<T>& target) : m_target(target) {
            
            }

        private:
            int parse(const std::string& s) override {
                std::stringstream ss;
                ss.str(s);
                T t;
                if (ss >> t){
                    m_target.emplace(std::move(t));
                    return 1;
                }
                return 0;
            }

            std::optional<T>& m_target;
        };

        std::vector<std::unique_ptr<Item>> m_items;
    };

    template<typename T>
    inline void ArgumentParser::add(T& target){
        m_items.push_back(std::make_unique<ConcreteItem<T>>(target));
    }

    template<typename T>
    inline void ArgumentParser::add(std::optional<T>& target){
        m_items.push_back(std::make_unique<ConcreteItem<std::optional<T>>>(target));
    }

} // namespace flui
