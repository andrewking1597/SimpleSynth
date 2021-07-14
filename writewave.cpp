#include <iostream>
#include <fstream>
#include <cmath>

void write_as_bytes(std::ofstream &file, int value, int byte_size);
double get_max_val(const double arr[], int size);
void normalize_array(double arr[], int size, const int max_amplitude=32760);
void array_tofile(const std::string fname, const double arr[], const int sample_rate=44100, const int num_channels=2, const int duration=3, const double frequency=440);
void write_wav(const std::string fname, const int sample_rate=44100, const int num_channels=2, const int duration=3, const double frequency=440);

int main() {

    write_wav("mywave.wav", 44100, 2, 3, 880);


    return 0;
}


/*
credit: https://www.youtube.com/watch?v=rHqkeLxAsTc (21:30)

convert numerical values to the correct size (b/c standard sizes vary depending on OS)
*/
void write_as_bytes(std::ofstream &file, int value, int byte_size) {
    file.write(reinterpret_cast<const char*>(&value), byte_size);
}

double get_max_val(const double arr[], int size) {
    // determine the max value in arr
    int i;
    double max_val = 0;
    for (i = 0; i < size; i++) {
        if (arr[i] > max_val) {
            max_val = arr[i];
        }
    }

    return max_val;
}

//todo function overload take an additional param const int max_val (more efficient if max_val is determined while array is being written)
void normalize_array(double arr[], int size, const int max_amplitude=32760) {

    // Get max value in arr
    double max_val = get_max_val(arr, size);

    // scale arr so max_val == max_amplitude
    for (int i = 0; i < size; i++) {
        arr[i] = arr[i] * max_amplitude / max_val;
    }

    return;
}

void array_tofile(const std::string fname, const double arr[], const int sample_rate=44100, const int num_channels=2, const int duration=3, const double frequency=440) {
    
    // Calculate array size
    const int size = sample_rate * duration;

    //* Set Header Values
    // RIFF chunk
    const std::string chunk_id = "RIFF";
    int chunk_size; // will be calculated after subchunk2_size
    const std::string format = "WAVE";
    
    // FMT sub-chunk
    const std::string subchunk1_id = "fmt ";
    const int subchunk1_size = 16; // 16 for PCM
    const int audio_format = 1; // 1 for PCM
    const int byte_rate = sample_rate * num_channels * (subchunk1_size/8);
    const int block_align = num_channels * (subchunk1_size/8);
    const int bits_per_sample = 16;

    // Data sub-chunk
    const std::string subchunk2_id = "data";
    int subchunk2_size = size * bits_per_sample / 8;
    const int max_amplitude = 32760; // because values are 16-bits

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
        for (int i = 0; i < size; i++) {
            write_as_bytes(wav, arr[i], 2);
        }
    }

    // Close file
    wav.close();


    return;
}

void write_wav(const std::string fname, const int sample_rate=44100, const int num_channels=2, const int duration=3, const double frequency=440) {

    // // keep track of max value being written
    // double max_val = 0;

    //* Write the data
    double data[sample_rate*duration*2];
        
    // Write audio data to data array
    for (int i = 0; i < sample_rate * duration; i++) {
        // double amplitude = (double)i / sample_rate * max_amplitude;
        double amplitude = 32760;
        double value = sin((2 * 3.14 * i * frequency) / sample_rate);

        double channel1 = amplitude * value;
        double channel2 = amplitude * value;

        // // if either channel value is greater than max_val: set as max_val
        // if (channel1 > max_val) {max_val = channel1;}
        // if (channel2 > max_val) {max_val = channel2;}

        // Write to array
        data[i*2] = channel1;
        data[i*2+1] = channel2;
    }

    // normalize array
    normalize_array(data, sample_rate*duration*2);

    // write data to wav file
    array_tofile(fname, data, sample_rate, num_channels, duration, frequency);

    return;
}
