from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeDeps, CMakeToolchain

class LiRi(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "sdl/2.32.10", "sdl_mixer/2.8.1", "openssl/1.1.1w"
    tool_requires = "cmake/4.2.1"

    def layout(self):
        cmake_layout(self)

    def configure(self):
        self.options["sdl_mixer/*"].mpg123 = False
        self.options["sdl_mixer/*"].opus = False
        self.options["sdl_mixer/*"].ogg = False
        self.options["sdl_mixer/*"].flac = False
        self.options["sdl_mixer/*"].mad = False

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.user_presets_path = 'ConanPresets.json'
        tc.generate()
