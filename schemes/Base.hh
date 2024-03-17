#ifndef COMPRESSOR_SCHEMES_BASE_HH
#define COMPRESSOR_SCHEMES_BASE_HH

#include <iostream>
#include <memory>

namespace Course
{
    class Compressor
    {
    protected:
        std::istream &m_input;
        std::ostream &m_output;

    public:
        Compressor() = default;
        Compressor(std::istream &input, std::ostream &output) : m_input(input), m_output(output) {}
        virtual ~Compressor() = default;

        void source(std::istream &input) { m_input.rdbuf(input.rdbuf()); }
        void output(std::ostream &output) { m_output.rdbuf(output.rdbuf()); }

        virtual void compress() = 0;
    };

    class Decompressor
    {
    protected:
        std::istream &m_input;
        std::ostream &m_output;

    public:
        Decompressor() = default;
        Decompressor(std::istream &input, std::ostream &output) : m_input(input), m_output(output) {}
        virtual ~Decompressor() = default;

        void source(std::istream &input) { m_input.rdbuf(input.rdbuf()); }
        void output(std::ostream &output) { m_output.rdbuf(output.rdbuf()); }

        virtual void decompress() = 0;
    };

    class Algorithm
    {
    public:
        virtual const char *name() const = 0;
        virtual std::unique_ptr<Compressor> getCompressor(std::istream &input, std::ostream &output) const = 0;
        virtual std::unique_ptr<Decompressor> getDecompressor(std::istream &input, std::ostream &output) const = 0;
    };
}

#endif