#include <meta>
#include <string>
#include <array>
#include <vector>

// TODO: figure out which one has std::begin/end/size
#include <iterator>
#include <ranges>

namespace CompileTimeJSON
{
    template <typename T>
    concept ToStringable =
        requires(T v) 
    {
        { std::to_string(v) } -> std::convertible_to<std::string>;
    };

    template<typename T>
    concept HasBeginEnd =
        requires(T t)
    {
        std::begin(t);
        std::end(t);
    };

    template <typename ValueT> constexpr std::string ToJSON(ValueT Value, size_t Indent = 4)
    {
        constexpr bool IsRefToClass = [] {
            if constexpr (std::is_reference_v<ValueT>)
            {
                if constexpr (std::is_class_v<typename [:std::meta::remove_reference(^^ValueT):]>)
                {
                    return true;
                }
            }
            return false;
        }();

        if constexpr (std::is_pointer_v<ValueT>)
        {
            return Value ? ToJSON<decltype(*Value)>(*Value, Indent) : "null";
        }
        else if constexpr (std::is_same_v<ValueT, bool>)
        {
            return Value ? "true" : "false";
        }
        else if constexpr (ToStringable<ValueT>)
        {
            return std::to_string(Value);
        }
        else if constexpr (std::is_same_v<std::string, ValueT>)
        {
            return '"' + Value + '"';
        }
        else if constexpr (std::is_same_v<std::string_view, ValueT>)
        {
            return '"' + std::string(Value) + '"';
        }
        else if constexpr (HasBeginEnd<ValueT>)
        {
            std::string IndentStr = "";
            std::string IndentStrMinusFour = "";

            for (size_t Idx = 0; Idx < Indent; Idx++)
            {
                IndentStr += " ";
            }

            for (size_t Idx = 0; Idx < Indent - 4; Idx++)
            {
                IndentStrMinusFour += " ";
            }

            std::string OutputResult = "[\n";

            size_t MemberIdx = 0;
            for (auto Child : Value)
            {
                OutputResult +=
                IndentStr
                + ToJSON<decltype(Child)>(Child, Indent + 4)
                + (MemberIdx == std::size(Value) - 1 ? "\n" : ",\n");

                MemberIdx++;
            }

            return OutputResult + IndentStrMinusFour + "]";
        }
        else if constexpr ((std::is_class_v<ValueT> or IsRefToClass) and not HasBeginEnd<ValueT>)
        {
            constexpr std::meta::info ValueTInfo = []{
                if constexpr (std::is_reference_v<ValueT>)
                {
                    return std::meta::remove_reference(^^ValueT);
                }
                else
                {
                    return ^^ValueT;
                }
            }();

            std::string IndentStr = "";
            std::string IndentStrMinusFour = "";

            for (size_t Idx = 0; Idx < Indent; Idx++)
            {
                IndentStr += " ";
            }

            for (size_t Idx = 0; Idx < Indent - 4; Idx++)
            {
                IndentStrMinusFour += " ";
            }

            std::string OutputResult = "{\n";

            constexpr auto Members = std::define_static_array(std::meta::nonstatic_data_members_of(ValueTInfo, std::meta::access_context::current()));

            size_t MemberIdx = 0;
            template for (constexpr std::meta::info Member : auto(Members))
            {
                OutputResult += 
                IndentStr
                + '"' 
                + std::string(std::meta::identifier_of(Member)) 
                + "\": " 
                + ToJSON<decltype(Value.[:Member:])>(Value.[:Member:], Indent + 4) 
                + (MemberIdx == std::size(Members) - 1 ? "\n" : ",\n");

                MemberIdx++;
            }

            return OutputResult + IndentStrMinusFour + "}";
        }
        else
        {
            return std::string("<invalid>");
        }
    }
}
