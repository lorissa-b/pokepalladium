# Sphinx configuration for the Pokémon Palladium docs site.
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import sys
from pathlib import Path

# Local extensions live in docs/_ext.
sys.path.insert(0, str(Path(__file__).parent / "_ext"))

project = "Pokémon Palladium"

# gen_pokedex and gen_moves regenerate their pages from src/data/ on every
# build, so the docs can never drift from the game data.
extensions = ["myst_parser", "gen_pokedex", "gen_moves"]
source_suffix = {".md": "markdown"}
exclude_patterns = ["_build", "_ext"]

html_theme = "sphinx_rtd_theme"
html_theme_options = {"navigation_depth": 3}
html_baseurl = "https://lorissa-b.github.io/pokepalladium/"

# "Edit on GitHub" links
html_context = {
    "display_github": True,
    "github_user": "lorissa-b",
    "github_repo": "pokepalladium",
    "github_version": "master",
    "conf_py_path": "/docs/",
}
