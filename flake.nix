{
  description = "MarbleMatcher";

  inputs.nixpkgs.url = "github:nixos/nixpkgs";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let 
          pkgs = nixpkgs.legacyPackages.${system};
          name = "MarbleMatcher";
          inputs = with pkgs; [ eigen sfml cmake gnumake libGL ];
          package = pkgs.stdenv.mkDerivation {
            name = name;
            src = self;
            buildInputs = inputs;
            buildPhase = ''
              cmake -DCMAKE_CXX_FLAGS="-I/usr/local/include" .
              cmake --build .
            '';
            installPhase = ''
              mkdir -p $out
              cp MarbleMarcher $out/
            '';
          };
        in
        {
          defaultApp = {
            type = "app";
            program = "${package}/MarbleMarcher";
          };
          devShell = pkgs.mkShell { buildInputs = inputs; };
        }
      );
}
