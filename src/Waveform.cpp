#include <vector>
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>

class Waveform {
    public:
        Waveform(double amp, double freq, const int rate, const int channels, const int dur) : sample_rate(rate), num_channels(channels), duration(dur) {
            this->amplitude = amp;
            this->frequency = freq;
        }
        void init_sine();
        void init_square();
        void init_triangle();
        void print_info(); //? Debugging
        int get_array_size();
        void to_wavfile(std::string fname);
        std::string get_wavetype();
    private:
        std::vector<double> data;
        double amplitude;
        double frequency;
        const int sample_rate;
        const int num_channels;
        const int duration;
        std::string wavetype;

        void normalize_array(const double max_val, const int max_amplitude=32760);
        void write_as_bytes(std::ofstream &file, int value, int byte_size);
};

void Waveform::init_sine() {
    /* Calculate sine wave values and store in data vector */

    // keep track of max value
    double max_value = 0; //? unnecessary b/c max value should always be amplitude + vertical shift
    double min_value = 0;
    double value;

    // Clear data vector
    this->data.clear();
    
    // loop from 0 -> sample_rate * duration
    for (int i = 0; i < this->sample_rate * this->duration; i++) {
        value = this->amplitude * sin((2 * 3.14 * i * this->frequency) / this->sample_rate);

        // push value for each channel
        for (int j = 0; j < this->num_channels; j++) {
            this->data.push_back(value);
        }

        // check max_value and update if needed
        if (value > max_value) {max_value = value;}
        if (value < min_value) {min_value = value;}
    }

    // Normalize array
    this->normalize_array(max_value);

    // Set wavetype to sine
    this->wavetype = "SINE";

    return;
}

void Waveform::init_square() {

    double value;
    int temp;

    // Clear data vector
    this->data.clear();

    for (int i = 0; i < this->sample_rate * this->duration; i++) {
        value = sin((2 * 3.14 * i * this->frequency) / this->sample_rate);
        if (value > 0) {
            value = 1;
        } else if (value < 0) {
            value = -1;
        } else {
            value = 0;
        }

        // push value to data array for each channel
        for (int j = 0; j < this->num_channels; j++) {
            this->data.push_back(value);
        }
    }

    // Normalize array
    this->normalize_array(1);

    // Set wavetype to square
    this->wavetype = "SQUARE";

    return;
}

void Waveform::to_wavfile(std::string fname) {
    /* Write a wav file using the data in data vector */
    
    //* Set Header Values
    // RIFF chunk
    const std::string chunk_id = "RIFF";
    int chunk_size; // will be calculated after subchunk2_size
    const std::string format = "WAVE";

    // FMT sub-chunk
    const std::string subchunk1_id = "fmt ";
    const int subchunk1_size = 16; // 16 for PCM
    const int audio_format = 1; // 1 for PCM
    const int byte_rate = this->sample_rate * this->num_channels * (subchunk1_size/8);
    const int block_align = this->num_channels * (subchunk1_size/8);
    const int bits_per_sample = 16;

    // Data sub-chunk
    const std::string subchunk2_id = "data";
    int subchunk2_size = this->data.size() * bits_per_sample / 8;
    // const int max_amplitude = 32760; // because values are 16-bits

    // calculate chunk_size
    chunk_size = 36 + subchunk2_size;

    //* Write to File
    // create ofstream object and open wav file
    std::ofstream wav;
    wav.open(fname, std::ios::binary);

    if (wav.is_open()) {
        // Write RIFF chunk descriptor
        wav << chunk_id;
        write_as_bytes(wav, chunk_size, 4);
        wav << format;

        // Write FMT sub-chunk
        wav << subchunk1_id;
        write_as_bytes(wav, subchunk1_size, 4);
        write_as_bytes(wav, audio_format, 2);
        write_as_bytes(wav, num_channels, 2);
        write_as_bytes(wav, sample_rate, 4);
        write_as_bytes(wav, byte_rate, 4);
        write_as_bytes(wav, block_align, 2);
        write_as_bytes(wav, bits_per_sample, 2);

        // Write Data sub-chunk
        wav << subchunk2_id;
        write_as_bytes(wav, subchunk2_size, 4);

        // write arr data
        for (int i = 0; i < this->data.size(); i++) {
            write_as_bytes(wav, this->data[i], 2);
        }
    }

    // Close file
    wav.close();
    
    return;
}

void Waveform::normalize_array(const double max_val, const int max_amplitude) {
    /* Scale values in data so the max value is equal to the max_amplitude allowed */

    for (int i = 0; i < this->data.size(); i++) {
        this->data[i] = this->data[i] * max_amplitude / max_val;
    }

    return;
}

int Waveform::get_array_size() {
    return this->data.size();
}

std::string Waveform::get_wavetype() {
    return this->wavetype;
}

void Waveform::write_as_bytes(std::ofstream &file, int value, int byte_size) {
    /*
    convert numerical values to the correct size (b/c standard sizes vary depending on OS)
    source: https://www.youtube.com/watch?v=rHqkeLxAsTc (21:30)
    */

    file.write(reinterpret_cast<const char*>(&value), byte_size);
}

//? Debugging
void Waveform::print_info() {
    std::cout << "Amplitude =    " << this->amplitude << std::endl;
    std::cout << "Frequency =    " << this->frequency << std::endl;
    std::cout << "Sample Rate =  " << this->sample_rate << std::endl;
    std::cout << "Num Channels = " << this->num_channels << std::endl;
    std::cout << "Duration =     " << this->duration << std::endl;

    return;
}
