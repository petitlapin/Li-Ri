from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeDeps, CMakeToolchain

class LiRi(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "sdl/3.4.8", "sdl_mixer/[~3.2.0]"#, "openssl/1.1.1w"

    def layout(self):
        cmake_layout(self)

    def configure(self):
        self.options["sdl_mixer/*"].with_mpg123 = False
        self.options["sdl_mixer/*"].with_opus = False
        #self.options["sdl_mixer/*"].ogg = False
        self.options["sdl_mixer/*"].with_flac = False
        #self.options["sdl_mixer/*"].mad = False

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.user_presets_path = 'ConanPresets.json'
        tc.generate()
