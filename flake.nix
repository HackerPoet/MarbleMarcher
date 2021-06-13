{
  description = "MarbleMatcher";

  inputs.nixpkgs.url = "github:nixos/nixpkgs";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let 
          pkgs = nixpkgs.legacyPackages.${system};
          inputs = with pkgs; [ eigen sfml cmake gnumake libGL ];
        in
        {
          devShell = pkgs.mkShell { buildInputs = inputs; };
        }
      );
}
