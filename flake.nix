{
  description = "Minimal Logos Module - Example using logos-module-builder";

  inputs = {
    logos-module-builder.url = "/home/vpavlin/devel/logos-module-builder-patched";
  };

  outputs = inputs@{ logos-module-builder, ... }:
    let
      # Main module derivation
      mainModule = logos-module-builder.lib.mkLogosModule {
        src = ./.;
        configFile = ./metadata.json;
        flakeInputs = inputs;
        postInstall = ''
          # Copy repc-generated headers to include dir
          if [ -d "./generated_code" ]; then
            mkdir -p $out/include
            cp ./generated_code/*.h $out/include/ 2>/dev/null || true
          fi
        '';
      };

      # Integration test derivation (T1.7)
      # Get pkgs from the same nixpkgs used by mkLogosModule
      testPkgs = import inputs.logos-module-builder.inputs.nixpkgs {
        system = "x86_64-linux";
      };
      testSearchClient = testPkgs.stdenv.mkDerivation {
        pname = "logos-ia-test-search-client";
        version = "0.1.0";
        src = ./.;

        nativeBuildInputs = with testPkgs;
          [ qt6.qtbase cmake pkg-config ];

        buildInputs = with testPkgs;
          [ qt6.qtbase qt6.qtdeclarative ];

        dontWrapQtApps = true;

        # Build using the tests/ subdirectory CMakeLists.txt
        configurePhase = ''
          cmake ../tests -B build -DCMAKE_CXX_STANDARD=17
        '';

        buildPhase = ''
          cmake --build build
        '';

        postInstall = ''
          mkdir -p $out/bin
          cp build/test_search_client $out/bin/
        '';
      };
    in
    {
      packages.x86_64-linux = {
        default = mainModule;
        test-search-client = testSearchClient;
      };
    };
}
