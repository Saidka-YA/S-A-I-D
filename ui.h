#ifndef UI_H
#define UI_H

#include <string>

class UserInterface {
public:
    void run();
private:
    std::string last_key_path;
    std::string last_input_path;
    std::string last_output_path;
    std::string last_algo;
    std::string last_save_text_path;
    
    void show_menu();
    void process_text(bool is_encrypt);
    void process_file(bool is_encrypt);
    void generate_and_save_key();
    
    std::string get_auto_text_filename(bool is_encrypt) const;
    std::string get_auto_output_path(const std::string& input_path, bool is_encrypt) const;
    
    std::string safe_input_string(const std::string& prompt);
    int safe_input_int(const std::string& prompt, int min_val, int max_val);
    std::string safe_input_path(const std::string& prompt);
    bool safe_input_yes_no(const std::string& prompt, bool default_yes = true);
};

#endif