#pragma once

#include <map>
#include <string>
#include <optional>
#include <functional>
#include <unordered_set>

#include "KeyValuePairExtractor.h"
#include "impl/state/State.h"
#include "impl/state/KeyStateHandler.h"
#include "impl/state/ValueStateHandler.h"
#include "KeyValuePairEscapingProcessor.h"

/*
 * Implements key value pair extraction by ignoring escaping and deferring its processing to the end.
 * This strategy allows more efficient memory usage in case of very noisy files because it does not have to
 * store characters while reading an element. Because of that, std::string_views can be used to store key value pairs.
 *
 * In the end, the unescaped key value pair views are converted into escaped key value pairs. At this stage, memory is allocated
 * to store characters, but noise is no longer an issue.
 * */
class LazyEscapingKeyValuePairExtractor : public KeyValuePairExtractor {
public:
    LazyEscapingKeyValuePairExtractor(KeyStateHandler keyStateHandler, ValueStateHandler valueStateHandler, KeyValuePairEscapingProcessor keyValuePairEscapingProcessor);

    [[nodiscard]] Response extract(const std::string & file) override;

private:
    NextState extract(const std::string & file, std::size_t pos, State state);

    NextState waitKey(const std::string & file, size_t pos) const;
    NextState readKey(const std::string & file, size_t pos);
    NextState readEnclosedKey(const std::string  &file, size_t pos);
    NextState readKeyValueDelimiter(const std::string & file, size_t pos) const;

    NextState waitValue(const std::string & file, size_t pos) const;
    NextState readValue(const std::string & file, size_t pos);
    NextState readEnclosedValue(const std::string & file, size_t pos);
    NextState readEmptyValue(const std::string & file, size_t pos);

    NextState flushPair(const std::string & file, std::size_t pos);

    KeyStateHandler keyStateHandler;
    ValueStateHandler valueStateHandler;
    KeyValuePairEscapingProcessor escapingProcessor;

    std::string_view key;
    std::string_view value;
    std::unordered_map<std::string_view, std::string_view> response_views;

};
