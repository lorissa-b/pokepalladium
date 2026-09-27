# Sphinx configuration for the Pokémon Palladium docs site.
# https://www.sphinx-doc.org/en/master/usage/configuration.html

project = "Pokémon Palladium"

extensions = ["myst_parser"]
source_suffix = {".md": "markdown"}
exclude_patterns = ["_build"]

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
