#include <meta>
#include <string>
#include <array>
#include <vector>

namespace CompileTimeJSON
{
    template <typename T>
    concept ToStringable =
        requires(T v) {
            { std::to_string(v) } -> std::convertible_to<std::string>;
        };

    template <typename ValueT, size_t Indent = 4> constexpr auto StructToJSON(const ValueT& Value)
    {
        if constexpr (ToStringable<ValueT>)
        {
            return std::to_string(Value);
        }
        else if constexpr (std::is_same_v<std::string, ValueT>)
        {
            return '"' + Value + '"';
        }
        else if constexpr (std::is_same_v<std::string_view, ValueT>)
        {
            return std::string(Value);
        }
        else
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

            std::string OutputResult = "{\n";

            constexpr auto Members = std::define_static_array(std::meta::nonstatic_data_members_of(^^ValueT, std::meta::access_context::current()));

            size_t MemberIdx = 0;
            template for (constexpr std::meta::info Member : auto(Members))
            {
                OutputResult += 
                IndentStr
                + '"' 
                + std::string(std::meta::identifier_of(Member)) 
                + "\": " 
                + StructToJSON<decltype(Value.[:Member:]), Indent + 4>(Value.[:Member:]) 
                + (MemberIdx == Members.size() - 1 ? "" : ",")
                + "\n";

                MemberIdx++;
            }

            return OutputResult + IndentStrMinusFour + "}";
        }
    }
}
