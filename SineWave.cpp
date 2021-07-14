#include <vector>
#include <iostream>
#include <cmath>

//todo write to file
//todo blend sine waves

class SineWave {
    public:
        SineWave(double amp, double freq, const int rate, const int channels, const int dur) : sample_rate(rate), num_channels(channels), duration(dur) {
            this->amplitude = amp;
            this->frequency = freq;

            this->init_wave();
        }
        void print_info();
        int get_array_size();

        //? debugging
        double get_max();

    private:
        std::vector<double> data;
        double amplitude;
        double frequency;
        const int sample_rate;
        const int num_channels;
        const int duration;

        void init_wave();
        void normalize_array(const double max_val, const int max_amplitude=32760);
};

//? debugging
double SineWave::get_max() {
    double maxval = 0;
    for (int i = 0; i < this->data.size(); i++) {
        if (this->data[i] > maxval) {
            maxval = this->data[i];
        }
    }

    return maxval;
}

void SineWave::init_wave() {

    // keep track of max value
    double max_value = 0;
    double value;
    
    // loop from 0 -> sample_rate * duration
    for (int i = 0; i < this->sample_rate * this->duration; i++) {
        value = this->amplitude * sin((2 * 3.14 * i * this->frequency) / this->sample_rate);

        // push value for each channel
        for (int j = 0; j < this->num_channels; j++) {
            this->data.push_back(value);
        }

        // check max_value and update if needed
        if (value > max_value) {max_value = value;}
    }

    // Normalize array
    this->normalize_array(max_value);

    return;
}

void SineWave::normalize_array(const double max_val, const int max_amplitude) {
    /* Scale values in data so the max value is equal to the max_amplitude allowed */

    for (int i = 0; i < this->data.size(); i++) {
        this->data[i] = this->data[i] * max_amplitude / max_val;
    }

    return;
}

int SineWave::get_array_size() {
    return this->data.size();
}

void SineWave::print_info() {
    std::cout << "Amplitude =    " << this->amplitude << std::endl;
    std::cout << "Frequency =    " << this->frequency << std::endl;
    std::cout << "Sample Rate =  " << this->sample_rate << std::endl;
    std::cout << "Num Channels = " << this->num_channels << std::endl;
    std::cout << "Duration =     " << this->duration << std::endl;
    std::cout << "Max Value =    " << this->get_max() << std::endl;

    return;
}
