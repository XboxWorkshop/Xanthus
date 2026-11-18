# Ubershaders

The fixed function shaders, their generators and the shared `states.jsonl`
file are all in XShaderRecomp:
`tools/XShaderRecomp/XShaderRecomp/fixed_function/`. 
The README there explains why the variants exist and how the selection works.

## Files

Relevant source code:

Generated variants, built from the shared `states.jsonl`:
- `generated/fixed_function_pixel_shader/`
- `generated/fixed_function_vertex_shader/`

Template selection code:
- `fixed_function_*_shader_variants.{h,cpp}`: capture state, select variants,
  and create shaders lazily.
- `ubershader_catalog.h`: shared selection and lazy creation machinery.

## Regenerating variants

The repo includes the generated variants; which means a regular build
never requires the execution of Python.

If a states file or a generator changes, CMake will stop the configuration and
shows the command to run:

```bash
python3 tools/XShaderRecomp/XShaderRecomp/fixed_function/fixed_function_pixel_shader/generate.py \
    --states tools/XShaderRecomp/XShaderRecomp/fixed_function/fixed_function_pixel_shader/states.jsonl \
    --output ShadowRecomp/gpu/ubershaders/generated/fixed_function_pixel_shader \
    --dxil-macro XANTHUS_D3D12

python3 tools/XShaderRecomp/XShaderRecomp/fixed_function/fixed_function_vertex_shader/generate.py \
    --states tools/XShaderRecomp/XShaderRecomp/fixed_function/fixed_function_vertex_shader/states.jsonl \
    --output ShadowRecomp/gpu/ubershaders/generated/fixed_function_vertex_shader \
    --dxil-macro XANTHUS_D3D12
```

You can then commit the regenerated `generated/` outputs together with the new changes.
