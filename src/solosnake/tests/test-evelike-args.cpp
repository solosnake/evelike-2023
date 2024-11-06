#include <array>
#include "solosnake/testing/testing.hpp"
#include "solosnake/evelike-args.hpp"
#include "solosnake/evelike-universe.hpp"

using namespace solosnake::evelike;

TEST( EvelikeArgs, get_help )
{
    std::cout << get_help("EveLike");
}

TEST( EvelikeArgs, bad_args )
{
    std::string a0{"EveLike"};
    std::string a1{"-xxx"};
    std::array<char*, 2> commands{ a0.data(), a1.data() };
    char** argv = commands.data();
    auto args = get_args( static_cast<int>(commands.size()), argv );
    EXPECT_FALSE( args );
}

TEST( EvelikeArgs, universe_file_time_limited )
{
    std::string a0{"EveLike"};
    std::string a1{"-t9876543"};
    std::string a2{"universe.json"};
    std::array<char*, 3> commands{ a0.data(), a1.data(), a2.data() };
    char** argv = commands.data();
    auto args = get_args( static_cast<int>(commands.size()), argv );
    EXPECT_TRUE( args );
    EXPECT_FALSE( args->debugging );
    EXPECT_EQ( args->max_runtime_ms, std::uint64_t{9876543u} * 1000ul );
    EXPECT_TRUE( args->file_names[0].ends_with("universe.json") );
}

TEST( EvelikeArgs, universe_file_debug_time_limited )
{
    std::string a0{"EveLike"};
    std::string a1{"-t3d"};
    std::string a2{"universe.json"};
    std::array<char*, 3> commands{ a0.data(), a1.data(), a2.data() };
    char** argv = commands.data();
    auto args = get_args( static_cast<int>(commands.size()), argv );
    EXPECT_TRUE( args );
    EXPECT_TRUE( args->debugging );
    EXPECT_EQ( args->max_runtime_ms, std::uint64_t{3} * 1000ul );
    EXPECT_TRUE( args->file_names[0].ends_with("universe.json") );
}

TEST( EvelikeArgs, make_skybox )
{
    std::string a0{"EveLike"};
    std::string a1{"-B"};
    std::string a2{"skybox1.sky"};
    std::string a3{"xp.png"};
    std::string a4{"xn.png"};
    std::string a5{"yp.png"};
    std::string a6{"yn.png"};
    std::string a7{"zp.png"};
    std::string a8{"zn.png"};
    std::array<char*, 9> commands{ a0.data(), a1.data(), a2.data(),
                                   a3.data(), a4.data(), a5.data(),
                                   a6.data(), a7.data(), a8.data() };
    char** argv = commands.data();
    auto args = get_args( static_cast<int>(commands.size()), argv );
    EXPECT_TRUE( args );
    EXPECT_FALSE( args->debugging );
    EXPECT_TRUE( args->file_names[0].ends_with("skybox1.sky") );
    EXPECT_TRUE( args->file_names[1].ends_with("xp.png") );
    EXPECT_TRUE( args->file_names[2].ends_with("xn.png") );
    EXPECT_TRUE( args->file_names[3].ends_with("yp.png") );
    EXPECT_TRUE( args->file_names[4].ends_with("yn.png") );
    EXPECT_TRUE( args->file_names[5].ends_with("zp.png") );
    EXPECT_TRUE( args->file_names[6].ends_with("zn.png") );
}

TEST( EvelikeArgs, bad_skybox )
{
    std::string a0{"EveLike"};
    std::string a1{"-B"};
    std::string a2{"skybox1.sky"};
    std::string a3{"xp.png"};
    std::string a4{"xn.png"};
    std::string a5{"yp.png"};
    std::string a6{"yn.png"};
    std::string a7{"zp.png"};
    std::array<char*, 8> commands{ a0.data(), a1.data(), a2.data(),
                                   a3.data(), a4.data(), a5.data(),
                                   a6.data(), a7.data() };
    char** argv = commands.data();
    auto args = get_args( static_cast<int>(commands.size()), argv );
    EXPECT_FALSE( args );
}

TEST( EvelikeArgs, show_model )
{
    std::string a0{"EveLike"};
    std::string a1{"-t200m"};
    std::string a2{"./test-assets/Mesh.msh"};
    std::array<char*, 3> commands{ a0.data(), a1.data(), a2.data() };
    char** argv = commands.data();
    auto args = get_args( static_cast<int>(commands.size()), argv );
    EXPECT_TRUE( args );
    EXPECT_TRUE( args->show_model );
    EXPECT_TRUE( args->file_names[0].ends_with("Mesh.msh") );
}

TEST( EvelikeArgs, show_model_with_skybox )
{
    std::string a0{"EveLike"};
    std::string a1{"-t200m"};
    std::string a2{"./test-assets/Mesh.msh"};
    std::string a3{"./test-assets/Skybox.sbx"};
    std::array<char*, 4> commands{ a0.data(), a1.data(), a2.data(), a3.data() };
    char** argv = commands.data();
    auto args = get_args( static_cast<int>(commands.size()), argv );
    EXPECT_TRUE( args );
    EXPECT_TRUE( args->show_model );
    EXPECT_TRUE( args->file_names[0].ends_with("Mesh.msh") );
    EXPECT_TRUE( args->file_names[1].ends_with("Skybox.sbx") );
}

TEST( EvelikeArgs, universe_file )
{
    std::string a0{"EveLike"};
    std::string a1{"universe.json"};
    std::array<char*, 2> commands{ a0.data(), a1.data() };
    char** argv = commands.data();
    auto args = get_args( static_cast<int>(commands.size()), argv );
    EXPECT_TRUE( args );
    EXPECT_TRUE( args->file_names[0].ends_with("universe.json") );
}