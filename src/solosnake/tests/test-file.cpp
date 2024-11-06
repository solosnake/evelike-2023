#include <algorithm>
#include <filesystem>
#include <fstream>
#include "solosnake/testing/testing.hpp"
#include "solosnake/file.hpp"

TEST( File, get_file_contents )
{
    // Temp file name.
    std::filesystem::path tmp_dir(std::filesystem::temp_directory_path());
    auto tmp_file = tmp_dir / "test_get_file_contents.txt";

    // Write file. Note that Windows adds "\r\n" for "\n" so comparisons can fail.
    const std::string test_text = "Hello World";

    std::ofstream out_file(tmp_file.c_str());
    out_file.exceptions( std::ios::failbit );
    out_file << test_text;
    out_file.close();

    // Read file contents.
    auto contents = solosnake::get_file_contents(tmp_file.string());

    EXPECT_EQ(contents, test_text);

    // Remove file
    std::filesystem::remove(tmp_file);
}
