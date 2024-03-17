#ifndef COMPRESSOR_SCHEMES_RUNLENGTH_HH
#define COMPRESSOR_SCHEMES_RUNLENGTH_HH

#include <Base.hh>
#include <cstdint>

namespace Course
{
    constexpr uint16_t MAX_RUN_LENGTH = 32768;

    struct Unit
    {
        uint8_t length;
        uint8_t data;
    } __attribute__((packed));

    struct Frame
    {
        uint16_t isRun : 1;
        uint16_t length : 15;
    } __attribute__((packed));

    class RunLengthCompressor : public Compressor
    {
    public:
        RunLengthCompressor(std::istream &input, std::ostream &output) : Compressor(input, output) {}

        uint16_t getBlock(uint8_t buffer[MAX_RUN_LENGTH])
        {
            // read MAX_RUN_LENGTH bytes or until EOF
            for (int i = 0; i < MAX_RUN_LENGTH; i++)
            {
                char c;
                if (m_input.get(c))
                    buffer[i] = c;
                else
                    return i;
            }

            return MAX_RUN_LENGTH;
        }

        void compressRun(uint8_t buffer[MAX_RUN_LENGTH], uint16_t inLen, uint8_t outBuf[MAX_RUN_LENGTH], uint16_t *outLen)
        {
            // For now just use a single unit
            Unit unit;

            // perform run length encoding
            uint16_t i = 0;
            while (i < inLen)
            {
                uint16_t j = i + 1;
                while (j < inLen && buffer[i] == buffer[j])
                    j++;

                unit.length = j - i;
                unit.data = buffer[i];

                outBuf[*outLen] = unit.length;
                outBuf[*outLen + 1] = unit.data;

                // if overflows, just return
                if (*outLen + 2 > MAX_RUN_LENGTH)
                {
                    *outLen = 0;
                    return;
                }

                *outLen += 2;

                i = j;
            }
        }

        bool isCompressable(uint8_t buffer[MAX_RUN_LENGTH], uint8_t compBuf[MAX_RUN_LENGTH], uint16_t inLen, uint16_t *outLen)
        {
            compressRun(buffer, inLen, compBuf, outLen);

            if (*outLen == 0)
                return false;
            else
                return *outLen + 2 < inLen;
        }

        void compress() override
        {
            /*
             * 1. Look ahead to determine if the next MAX_RUN_LENGTH bytes could be compressed
             * 2. If not, write a frame with isRun = 0 and length = MAX_RUN_LENGTH or less and the data
             * 3. If so, write a frame with isRun = 1 and length = MAX_RUN_LENGTH or less and compress the run into units
             * 4. Repeat until the end of the input
             */

            uint8_t runBuffer[MAX_RUN_LENGTH], compBuffer[MAX_RUN_LENGTH];
            uint16_t outLen;

            while (true)
            {
                uint16_t bytesRead = getBlock(runBuffer);
                if (bytesRead == 0)
                    break;

                if (isCompressable(runBuffer, compBuffer, bytesRead, &outLen))
                {
                    std::cout << "Compressing run of length " << bytesRead << " to " << outLen << std::endl;
                    Frame frame;
                    frame.isRun = 1;
                    frame.length = outLen;
                    m_output.write(reinterpret_cast<char *>(&frame), sizeof(Frame));
                    m_output.write(reinterpret_cast<char *>(compBuffer), outLen);
                }
                else
                {
                    Frame frame;
                    frame.isRun = 0;
                    frame.length = bytesRead;
                    m_output.write(reinterpret_cast<char *>(&frame), sizeof(Frame));
                    m_output.write(reinterpret_cast<char *>(runBuffer), bytesRead);
                }
            }
        }
    };

    class RunLengthDecompressor : public Decompressor
    {
    public:
        RunLengthDecompressor(std::istream &input, std::ostream &output) : Decompressor(input, output) {}

        void decompress() override
        {
            Frame frame;
            while (m_input.read(reinterpret_cast<char *>(&frame), sizeof(Frame)))
            {
                if (frame.isRun)
                {
                    Unit unit;
                    for (int i = 0; i < frame.length; i += 2)
                    {
                        m_input.read(reinterpret_cast<char *>(&unit), sizeof(Unit));
                        for (int j = 0; j < unit.length; j++)
                            m_output.put(unit.data);
                    }
                }
                else
                {
                    char buffer[MAX_RUN_LENGTH];
                    m_input.read(buffer, frame.length);
                    m_output.write(buffer, frame.length);
                }
            }
        }
    };

    class RunLengthAlgorithm : public Algorithm
    {
    public:
        const char *name() const override { return "runlength"; }

        std::unique_ptr<Compressor> getCompressor(std::istream &input, std::ostream &output) const override
        {
            return std::make_unique<RunLengthCompressor>(input, output);
        }

        std::unique_ptr<Decompressor> getDecompressor(std::istream &input, std::ostream &output) const override
        {
            return std::make_unique<RunLengthDecompressor>(input, output);
        }
    };
};

#endif