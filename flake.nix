{
  description = "Minimal Logos Module - Example using logos-module-builder";

  inputs = {
    logos-module-builder.url = "/home/vpavlin/devel/logos-module-builder-patched";
  };

  outputs = inputs@{ logos-module-builder, ... }:
    logos-module-builder.lib.mkLogosModule {
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
}
