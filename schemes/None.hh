#ifndef COMPRESSOR_SCHEMES_NONE_HH
#define COMPRESSOR_SCHEMES_NONE_HH

#include <Base.hh>

namespace Course
{
    class NoneCompressor : public Compressor
    {
    public:
        NoneCompressor(std::istream &input, std::ostream &output) : Compressor(input, output) {}

        void compress() override
        {
            char c;
            while (m_input.get(c))
            {
                m_output.put(c);
            }
        }
    };

    class NoneDecompressor : public Decompressor
    {
    public:
        NoneDecompressor(std::istream &input, std::ostream &output) : Decompressor(input, output) {}

        void decompress() override
        {
            char c;
            while (m_input.get(c))
            {
                m_output.put(c);
            }
        }
    };

    class NoneAlgorithm : public Algorithm
    {
    public:
        const char *name() const override { return "none"; }

        std::unique_ptr<Compressor> getCompressor(std::istream &input, std::ostream &output) const override
        {
            return std::make_unique<NoneCompressor>(input, output);
        }

        std::unique_ptr<Decompressor> getDecompressor(std::istream &input, std::ostream &output) const override
        {
            return std::make_unique<NoneDecompressor>(input, output);
        }
    };
}

#endif