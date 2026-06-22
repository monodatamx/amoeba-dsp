#pragma once

#include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string>

namespace myseq::presets {

template <std::size_t ParameterCount, std::size_t SlotCount = 16>
class PresetBank {
public:
    using Values = std::array<double, ParameterCount>;

    struct Slot {
        bool valid = false;
        std::string name;
        Values values {};
    };

    bool store(std::size_t index, const Values& values, const std::string& name = {})
    {
        if (index >= SlotCount) return false;
        slots_[index].valid = true;
        slots_[index].values = values;
        slots_[index].name = name.empty() ? "Preset " + std::to_string(index + 1) : name.substr(0, 63);
        return true;
    }

    bool recall(std::size_t index, Values& values) const
    {
        if (index >= SlotCount || !slots_[index].valid) return false;
        values = slots_[index].values;
        return true;
    }

    bool clear(std::size_t index)
    {
        if (index >= SlotCount) return false;
        slots_[index] = Slot {};
        return true;
    }

    [[nodiscard]] bool valid(std::size_t index) const { return index < SlotCount && slots_[index].valid; }
    [[nodiscard]] const std::string& name(std::size_t index) const
    {
        static const std::string empty;
        return index < SlotCount ? slots_[index].name : empty;
    }

    bool save(const std::filesystem::path& path) const
    {
        std::error_code error;
        const auto parent = path.parent_path();
        if (!parent.empty()) std::filesystem::create_directories(parent, error);
        if (error) return false;
        std::ofstream output(path, std::ios::trunc);
        if (!output) return false;
        output << "MYSEQ_PRESETS 1 " << ParameterCount << ' ' << SlotCount << '\n';
        output << std::setprecision(17);
        for (std::size_t i = 0; i < SlotCount; ++i) {
            if (!slots_[i].valid) continue;
            output << "SLOT " << i << ' ' << std::quoted(slots_[i].name);
            for (double value : slots_[i].values) output << ' ' << value;
            output << '\n';
        }
        return static_cast<bool>(output);
    }

    bool load(const std::filesystem::path& path)
    {
        std::ifstream input(path);
        if (!input) return false;
        std::string magic;
        std::size_t version = 0, parameters = 0, slots = 0;
        input >> magic >> version >> parameters >> slots;
        if (!input || magic != "MYSEQ_PRESETS" || version != 1 || parameters != ParameterCount || slots != SlotCount) return false;
        std::array<Slot, SlotCount> loaded {};
        std::string token;
        while (input >> token) {
            if (token != "SLOT") return false;
            std::size_t index = 0;
            std::string name;
            input >> index >> std::quoted(name);
            if (!input || index >= SlotCount) return false;
            loaded[index].valid = true;
            loaded[index].name = name;
            for (double& value : loaded[index].values) input >> value;
            if (!input) return false;
        }
        slots_ = std::move(loaded);
        return true;
    }

    static std::filesystem::path defaultPath(const std::string& instrument)
    {
        const char* home = std::getenv("HOME");
        std::filesystem::path base = home && *home ? std::filesystem::path(home) : std::filesystem::current_path();
        return base / ".myseq" / "presets" / (instrument + ".bank");
    }

private:
    std::array<Slot, SlotCount> slots_ {};
};

} // namespace myseq::presets
