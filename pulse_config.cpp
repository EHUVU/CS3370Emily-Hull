/*
Emily Hull

This assigment took a bit.

I am glad that each section was divided into layers because there are a lot of moving parts here.

Overall, this assignment was interesting. It is a nice refresh on string manipulation as well.
*/

#include "pulse_config.h"

#include <cmath>
#include <fstream>

//----------LAYER 1

/*
Parse integer and catch invalid argument errors for both NotANumber and OutOfRange.
*/
std::expected<int, ParseError> parse_int(std::string_view s)
{
    try
    {
        std::string str{s};
        std::size_t pos = 0;
        int n = std::stoi(str, &pos);

        if (pos != str.size())
        {
            return std::unexpected(ParseError::NotANumber);
        }

        return n;
    }
    catch (const std::invalid_argument &)
    {
        return std::unexpected(ParseError::NotANumber);
    }
    catch (const std::out_of_range &)
    {
        return std::unexpected(ParseError::OutOfRange);
    }
}

/*
Parse double and catch invalid argument errors for both NotANumber and OutOfRange.
*/
std::expected<double, ParseError> parse_double(std::string_view s)
{
    try
    {
        std::string str{s};
        std::size_t pos = 0;
        double n = std::stod(str, &pos);

        if (pos != str.size())
        {
            return std::unexpected(ParseError::NotANumber);
        }

        return n;
    }
    catch (const std::invalid_argument &)
    {
        return std::unexpected(ParseError::NotANumber);
    }
    catch (const std::out_of_range &)
    {
        return std::unexpected(ParseError::OutOfRange);
    }
}

//---------------LAYER 2

/*
My helper function to trim the whitespace from the section of text.
*/
std::string_view trimmer(std::string_view text)
{
    std::string_view whitespace = " \t\r\n";
    std::size_t first = text.find_first_not_of(whitespace);

    if (first == std::string_view::npos)
    {
        return "";
    }

    std::size_t last = text.find_last_not_of(whitespace);

    return text.substr(first, last - first + 1);
}

/*
Parses through the text, catches if the text is empty or contains a comment symbol #, trims the key and value for any
whitespace and returns a KeyValue struct.
*/
std::expected<std::optional<KeyValue>, ConfigError>
parse_line(std::string_view text, int line_no)
{
    text = trimmer(text);
    if (text.empty() || text.starts_with("#"))
    {
        return std::optional<KeyValue>{};
    }
    std::size_t separator = text.find('=');
    if (separator == std::string_view::npos)
    {
        return std::unexpected(ConfigError{ConfigErrc::MalformedLine, std::string{text}, line_no});
    }

    std::string key{trimmer(text.substr(0, separator))};
    std::string value{trimmer(text.substr(separator + 1))};

    if (key.empty())
    {
        return std::unexpected(ConfigError{ConfigErrc::MalformedLine, std::string{text}, line_no});
    }

    return std::optional<KeyValue>{KeyValue{key, value, line_no}};
}

//-----------LAYER 3

/*
Before setting the parameters, checks to see if the key name already exists. If not, set the parameter.
*/
std::expected<void, ConfigError> RawParams::set(const KeyValue &kv)
{
    if (kv.key == "vt")
    {
        if (vt)
        {
            return std::unexpected(ConfigError{ConfigErrc::DuplicateKey, kv.key, kv.line});
        }
        vt = kv.value;
    }
    else if (kv.key == "width")
    {
        if (width)
        {
            return std::unexpected(ConfigError{ConfigErrc::DuplicateKey, kv.key, kv.line});
        }
        width = kv.value;
    }
    else if (kv.key == "pulse_delta")
    {
        if (pulse_delta)
        {
            return std::unexpected(ConfigError{ConfigErrc::DuplicateKey, kv.key, kv.line});
        }
        pulse_delta = kv.value;
    }
    else if (kv.key == "drop_ratio")
    {
        if (drop_ratio)
        {
            return std::unexpected(ConfigError{ConfigErrc::DuplicateKey, kv.key, kv.line});
        }
        drop_ratio = kv.value;
    }
    else if (kv.key == "below_drop_ratio")
    {
        if (below_drop_ratio)
        {
            return std::unexpected(ConfigError{ConfigErrc::DuplicateKey, kv.key, kv.line});
        }
        below_drop_ratio = kv.value;
    }
    else
    {
        return std::unexpected(ConfigError{ConfigErrc::UnknownKey, kv.key, kv.line});
    }

    return {};
}

/*
Reads the input stream line by line, if successful, sets the raw parameters and returns the struct.
*/
std::expected<RawParams, ConfigError> read_raw(std::istream &in)
{
    RawParams raw;
    std::string line;
    int lineNum = 0;
    while (std::getline(in, line))
    {
        ++lineNum;
        if (line.empty() || line.starts_with("#"))
        {
            continue;
        }
        auto result = parse_line(line, lineNum);
        if (!result)
        {
            return std::unexpected(result.error());
        }
        if (!result.value().has_value())
        {
            continue;
        }

        KeyValue kv = result.value().value();

        auto rawCheck = raw.set(kv);

        if (!rawCheck)
        {
            return std::unexpected(rawCheck.error());
        }
    }
    return raw;
}

/*
    validates the parameters in an existing raw struct. If successful, a PulseParams object is created with the
    validated raw parameters. Returns the newly created PulseParams upon success. If it fails, it will return
    an OutOfRange, NotANumber or a MissingKeys error.
*/
std::expected<PulseParams, ConfigError>
validate(const RawParams &raw)
{
    if (!raw.vt || !raw.width || !raw.pulse_delta || !raw.drop_ratio || !raw.below_drop_ratio)
    {
        return std::unexpected(ConfigError{ConfigErrc::MissingKeys, "", 0});
    }

    auto vt = parse_int((*raw.vt));
    if (!vt)
    {
        if (vt.error() == ParseError::OutOfRange)
        {
            return std::unexpected(ConfigError{ConfigErrc::OutOfRange, "vt", 0});
        }
        return std::unexpected(ConfigError{ConfigErrc::NotANumber, "vt", 0});
    }
    auto width = parse_int((*raw.width));
    if (!width)
    {
        return std::unexpected(ConfigError{ConfigErrc::NotANumber, "width", 0});
    }
    auto pulse_delta = parse_int((*raw.pulse_delta));
    if (!pulse_delta)
    {
        return std::unexpected(ConfigError{ConfigErrc::NotANumber, "pulse_delta", 0});
    }
    auto drop_ratio = parse_double((*raw.drop_ratio));
    if (!drop_ratio)
    {
        if (drop_ratio.error() == ParseError::OutOfRange)
        {
            return std::unexpected(ConfigError{ConfigErrc::OutOfRange, "drop_ratio", 0});
        }
        return std::unexpected(ConfigError{ConfigErrc::NotANumber, "drop_ratio", 0});
    }

    auto below_drop_ratio = parse_int((*raw.below_drop_ratio));

    if (!below_drop_ratio)
    {
        return std::unexpected(ConfigError{ConfigErrc::NotANumber, "below_drop_ratio", 0});
    }

    if (*vt <= 0)
    {
        return std::unexpected(ConfigError{ConfigErrc::OutOfRange, "vt", 0});
    }
    if (*width <= 0)
    {
        return std::unexpected(ConfigError{ConfigErrc::OutOfRange, "width", 0});
    }
    if (*pulse_delta <= 0)
    {
        return std::unexpected(ConfigError{ConfigErrc::OutOfRange, "pulse_delta", 0});
    }
    if (!std::isfinite(*drop_ratio) || *drop_ratio <= 0.0 || *drop_ratio >= 1.0)
    {
        return std::unexpected(ConfigError{ConfigErrc::OutOfRange, "drop_ratio", 0});
    }
    if (*below_drop_ratio < 0)
    {
        return std::unexpected(ConfigError{ConfigErrc::OutOfRange, "below_drop_ratio", 0});
    }

    PulseParams pp{
        .vt = *vt,
        .width = *width,
        .pulse_delta = *pulse_delta,
        .drop_ratio = *drop_ratio,
        .below_drop_ratio = *below_drop_ratio};

    return pp;
}

/*
Reads input stream and checks if the raw is validated. Returns rawPass if successful.
*/
std::expected<PulseParams, ConfigError>
load_params(std::istream &in)
{
    auto raw = read_raw(in);
    if (!raw)
    {
        return std::unexpected(raw.error());
    }
    auto rawPass = validate(raw.value());
    if (!rawPass)
    {
        return std::unexpected(rawPass.error());
    }
    return rawPass;
}

//-----LAYER 4
/*
Error codes that will be returned for each specific error.
*/
std::string to_message(const ConfigError &err, std::string_view filename)
{
    switch (err.code)
    {
    case ConfigErrc::CannotOpen:
        return "Cannot open INI file: " + std::string(filename);
        break;
    case ConfigErrc::DuplicateKey:
        return "Invalid or duplicate INI key: " + err.detail;
        break;
    case ConfigErrc::MalformedLine:
        return "Malformed line " + std::to_string(err.line) + " in " + std::string(filename);
        break;
    case ConfigErrc::MissingKeys:
        return "Invalid INI file: missing one or more keys";
        break;
    case ConfigErrc::NotANumber:
        return "Invalid value for " + err.detail + " in " + std::string(filename);
        break;
    case ConfigErrc::OutOfRange:
        return "Invalid value for " + err.detail + " in " + std::string(filename);
        break;
    case ConfigErrc::UnknownKey:
        return "Invalid or duplicate INI key: " + err.detail;
        break;
    }
    return "Unknown Error";
}

/*
Constructor for ConfigFileError
*/
ConfigFileError::ConfigFileError(ConfigError err, std::string_view filename) : std::runtime_error(to_message(err, filename)), err_(std::move(err)) {}

/*
Constructor for pulseConfig
*/
PulseConfig::PulseConfig(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        throw ConfigFileError(ConfigError(ConfigErrc::CannotOpen, filename, 0), filename);
    }

    auto ldParam = load_params(file);
    if (!ldParam)
    {
        throw ConfigFileError(ldParam.error(), filename);
    }

    params_ = ldParam.value();
}
