use std::{
    env,
    error::Error,
    fs,
    path::{Path, PathBuf},
};

fn main() -> Result<(), Box<dyn Error>> {
    let root = root_dir()?;
    let include = root.join("include");
    let src = root.join("src");

    build(&include, &src)?;

    let bindings = generate_bindings(&include)?;
    write_bindings(&bindings)?;

    Ok(())
}

fn build(include_dir: &Path, src_dir: &Path) -> Result<(), Box<dyn Error>> {
    let mut build = cc::Build::new();

    build.include(include_dir);

    // Suppress unused parameter warnings from C code
    build.flag_if_supported("-Wno-unused-parameter");

    build.files(source_files(src_dir)?);
    build.try_compile("rbs")?;

    Ok(())
}

fn root_dir() -> Result<PathBuf, Box<dyn Error>> {
    Ok(Path::new(env!("CARGO_MANIFEST_DIR"))
        .ancestors()
        .nth(2)
        .ok_or("Failed to find project root directory")?
        .to_path_buf())
}

fn source_files<P: AsRef<Path>>(root_dir: P) -> Result<Vec<String>, Box<dyn Error>> {
    let mut files = Vec::new();

    for entry in fs::read_dir(root_dir.as_ref()).map_err(|e| {
        format!(
            "Failed to read source directory {:?}: {}",
            root_dir.as_ref(),
            e
        )
    })? {
        let entry = entry.map_err(|e| format!("Failed to read directory entry: {}", e))?;
        let path = entry.path();

        if path.is_file() {
            let path_str = path
                .to_str()
                .ok_or_else(|| format!("Invalid UTF-8 in filename: {:?}", path))?;

            if Path::new(path_str)
                .extension()
                .is_some_and(|ext| ext.eq_ignore_ascii_case("c"))
            {
                files.push(path_str.to_string());
            }
        } else if path.is_dir() {
            files.extend(source_files(path)?);
        }
    }

    Ok(files)
}

fn generate_bindings(include_path: &Path) -> Result<bindgen::Bindings, Box<dyn Error>> {
    let bindings = bindgen::Builder::default()
        .header("wrapper.h")
        .clang_arg(format!("-I{}", include_path.display()))
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        .generate_comments(true)
        // Core types
        .allowlist_type("rbs_signature_t")
        .allowlist_type("rbs_encoding_t")
        .allowlist_type("rbs_encoding_type_t")
        .allowlist_type("rbs_string_t")
        .constified_enum_module("rbs_encoding_type_t")
        // Encodings
        .allowlist_var("rbs_encodings")
        // Parser functions
        .allowlist_function("rbs_parser_new")
        .allowlist_function("rbs_parser_free")
        .allowlist_function("rbs_parse_signature")
        // String functions
        .allowlist_function("rbs_string_new")
        // Global constant pool
        .allowlist_var("RBS_GLOBAL_CONSTANT_POOL")
        .allowlist_function("rbs_constant_pool_init")
        .allowlist_function("rbs_constant_pool_free")
        .generate()
        .map_err(|_| "Unable to generate rbs bindings")?;

    Ok(bindings)
}

fn write_bindings(bindings: &bindgen::Bindings) -> Result<(), Box<dyn Error>> {
    let out_path = PathBuf::from(
        env::var("OUT_DIR").map_err(|e| format!("OUT_DIR environment variable not set: {}", e))?,
    );

    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .map_err(|e| {
            format!(
                "Failed to write bindings to {:?}: {}",
                out_path.join("bindings.rs"),
                e
            )
        })?;

    Ok(())
}
