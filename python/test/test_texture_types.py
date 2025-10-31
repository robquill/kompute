import os
import pytest
import kp
import numpy as np

from .utils import compile_source

def test_type_float():

    shader = """
        #version 450
        layout(set = 0, binding = 0) uniform sampler2D valuesLhs;
        layout(set = 0, binding = 1) uniform sampler2D valuesRhs;
        layout(set = 0, binding = 2, r32f) uniform image2D imageOutput;
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

        void main()
        {
            uint index = gl_GlobalInvocationID.x;
            ivec2 outSize = imageSize(imageOutput);

            // Normalized coordinates for texture()
            vec2 uv = (vec2(index) + 0.5) / vec2(outSize);

            imageStore(imageOutput, ivec2(index, 0), texture(valuesLhs, uv) * texture(valuesRhs, uv));
        }
    """

    spirv = compile_source(shader)

    arr_in_a = np.array([123., 153., 231.], dtype=np.float32)
    arr_in_b = np.array([9482, 1208, 1238], dtype=np.float32)
    arr_out = np.array([0, 0, 0], dtype=np.float32)

    mgr = kp.Manager()

    texture_in_a = mgr.texture(arr_in_a, 3, 1, 1, filter=kp.Filter.eNearest, sampler_address_mode=kp.SamplerAddressMode.eRepeat)
    texture_in_b = mgr.texture(arr_in_b, 3, 1, 1, filter=kp.Filter.eNearest, sampler_address_mode=kp.SamplerAddressMode.eRepeat)
    image_out = mgr.image(arr_out, 3, 1, 1)

    params = [texture_in_a, texture_in_b, image_out]

    (mgr.sequence()
        .record(kp.OpSyncDevice(params))
        .record(kp.OpAlgoDispatch(mgr.algorithm(params, spirv)))
        .record(kp.OpSyncLocal([image_out]))
        .eval())

    assert np.all(image_out.data() == arr_in_a * arr_in_b)

def test_type_int():

    shader = """
        #version 450
        layout(set = 0, binding = 0) uniform sampler2D valuesLhs;
        layout(set = 0, binding = 1) uniform sampler2D valuesRhs;
        layout(set = 0, binding = 2, r32i) uniform image2D imageOutput;
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

        void main()
        {
            uint index = gl_GlobalInvocationID.x;
            ivec2 outSize = imageSize(imageOutput);

            // Normalized coordinates for texture()
            vec2 uv = (vec2(index) + 0.5) / vec2(outSize);

            imageStore(imageOutput, ivec2(index, 0), texture(valuesLhs, uv) * texture(valuesRhs, uv));
        }
    """

    spirv = compile_source(shader)

    arr_in_a = np.array([123, 153, 231], dtype=np.int32)
    arr_in_b = np.array([9482, 1208, 1238], dtype=np.int32)
    arr_out = np.array([0, 0, 0], dtype=np.int32)

    mgr = kp.Manager()

    texture_in_a = mgr.texture(arr_in_a, 3, 1, 1, filter=kp.Filter.eNearest, sampler_address_mode=kp.SamplerAddressMode.eRepeat)
    texture_in_b = mgr.texture(arr_in_b, 3, 1, 1, filter=kp.Filter.eNearest, sampler_address_mode=kp.SamplerAddressMode.eRepeat)
    image_out = mgr.image(arr_out, 3, 1, 1)

    params = [texture_in_a, texture_in_b, image_out]

    (mgr.sequence()
        .record(kp.OpSyncDevice(params))
        .record(kp.OpAlgoDispatch(mgr.algorithm(params, spirv)))
        .record(kp.OpSyncLocal([image_out]))
        .eval())

    assert np.all(image_out.data() == arr_in_a * arr_in_b)

def test_type_unsigned_int():

    shader = """
        #version 450
        layout(set = 0, binding = 0) uniform sampler2D valuesLhs;
        layout(set = 0, binding = 1) uniform sampler2D valuesRhs;
        layout(set = 0, binding = 2, r32ui) uniform image2D imageOutput;
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

        void main()
        {
            uint index = gl_GlobalInvocationID.x;
            ivec2 outSize = imageSize(imageOutput);

            // Normalized coordinates for texture()
            vec2 uv = (vec2(index) + 0.5) / vec2(outSize);

            imageStore(imageOutput, ivec2(index, 0), texture(valuesLhs, uv) * texture(valuesRhs, uv));
        }
    """

    spirv = compile_source(shader)

    arr_in_a = np.array([123, 153, 231], dtype=np.uint32)
    arr_in_b = np.array([9482, 1208, 1238], dtype=np.uint32)
    arr_out = np.array([0, 0, 0], dtype=np.uint32)

    mgr = kp.Manager()

    texture_in_a = mgr.texture(arr_in_a, 3, 1, 1, filter=kp.Filter.eNearest, sampler_address_mode=kp.SamplerAddressMode.eRepeat)
    texture_in_b = mgr.texture(arr_in_b, 3, 1, 1, filter=kp.Filter.eNearest, sampler_address_mode=kp.SamplerAddressMode.eRepeat)
    image_out = mgr.image(arr_out, 3, 1, 1)

    params = [texture_in_a, texture_in_b, image_out]

    (mgr.sequence()
        .record(kp.OpSyncDevice(params))
        .record(kp.OpAlgoDispatch(mgr.algorithm(params, spirv)))
        .record(kp.OpSyncLocal([image_out]))
        .eval())

    assert np.all(image_out.data() == arr_in_a * arr_in_b)

def test_type_short():

    shader = """
        #version 450
        layout(set = 0, binding = 0) uniform sampler2D valuesLhs;
        layout(set = 0, binding = 1) uniform sampler2D valuesRhs;
        layout(set = 0, binding = 2, r16i) uniform image2D imageOutput;
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

        void main()
        {
            uint index = gl_GlobalInvocationID.x;
            ivec2 outSize = imageSize(imageOutput);

            // Normalized coordinates for texture()
            vec2 uv = (vec2(index) + 0.5) / vec2(outSize);

            imageStore(imageOutput, ivec2(index, 0), texture(valuesLhs, uv) * texture(valuesRhs, uv));
        }
    """

    spirv = compile_source(shader)

    arr_in_a = np.array([12, 15, 23], dtype=np.int16)
    arr_in_b = np.array([948, 120, 123], dtype=np.int16)
    arr_out = np.array([0, 0, 0], dtype=np.int16)

    mgr = kp.Manager()

    texture_in_a = mgr.texture(arr_in_a, 3, 1, 1, filter=kp.Filter.eNearest, sampler_address_mode=kp.SamplerAddressMode.eRepeat)
    texture_in_b = mgr.texture(arr_in_b, 3, 1, 1, filter=kp.Filter.eNearest, sampler_address_mode=kp.SamplerAddressMode.eRepeat)
    image_out = mgr.image(arr_out, 3, 1, 1)

    params = [texture_in_a, texture_in_b, image_out]

    (mgr.sequence()
        .record(kp.OpSyncDevice(params))
        .record(kp.OpAlgoDispatch(mgr.algorithm(params, spirv)))
        .record(kp.OpSyncLocal([image_out]))
        .eval())

    assert np.all(image_out.data() == arr_in_a * arr_in_b)

def test_type_unsigned_short():

    shader = """
        #version 450
        layout(set = 0, binding = 0) uniform sampler2D valuesLhs;
        layout(set = 0, binding = 1) uniform sampler2D valuesRhs;
        layout(set = 0, binding = 2, r16ui) uniform image2D imageOutput;
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

        void main()
        {
            uint index = gl_GlobalInvocationID.x;
            ivec2 outSize = imageSize(imageOutput);

            // Normalized coordinates for texture()
            vec2 uv = (vec2(index) + 0.5) / vec2(outSize);

            imageStore(imageOutput, ivec2(index, 0), texture(valuesLhs, uv) * texture(valuesRhs, uv));
        }
    """

    spirv = compile_source(shader)

    arr_in_a = np.array([12, 15, 23], dtype=np.uint16)
    arr_in_b = np.array([948, 120, 123], dtype=np.uint16)
    arr_out = np.array([0, 0, 0], dtype=np.uint16)

    mgr = kp.Manager()

    texture_in_a = mgr.texture(arr_in_a, 3, 1, 1, filter=kp.Filter.eNearest, sampler_address_mode=kp.SamplerAddressMode.eRepeat)
    texture_in_b = mgr.texture(arr_in_b, 3, 1, 1, filter=kp.Filter.eNearest, sampler_address_mode=kp.SamplerAddressMode.eRepeat)
    image_out = mgr.image(arr_out, 3, 1, 1)

    params = [texture_in_a, texture_in_b, image_out]

    (mgr.sequence()
        .record(kp.OpSyncDevice(params))
        .record(kp.OpAlgoDispatch(mgr.algorithm(params, spirv)))
        .record(kp.OpSyncLocal([image_out]))
        .eval())

    assert np.all(image_out.data() == arr_in_a * arr_in_b)

def test_type_char():

    shader = """
        #version 450
        layout(set = 0, binding = 0) uniform sampler2D valuesLhs;
        layout(set = 0, binding = 1) uniform sampler2D valuesRhs;
        layout(set = 0, binding = 2, r8i) uniform image2D imageOutput;
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

        void main()
        {
            uint index = gl_GlobalInvocationID.x;
            ivec2 outSize = imageSize(imageOutput);

            // Normalized coordinates for texture()
            vec2 uv = (vec2(index) + 0.5) / vec2(outSize);

            imageStore(imageOutput, ivec2(index, 0), texture(valuesLhs, uv) * texture(valuesRhs, uv));
        }
    """

    spirv = compile_source(shader)

    arr_in_a = np.array([2, 3, 2], dtype=np.int8)
    arr_in_b = np.array([35, 12, 23], dtype=np.int8)
    arr_out = np.array([0, 0, 0], dtype=np.int8)

    mgr = kp.Manager()

    texture_in_a = mgr.texture(arr_in_a, 3, 1, 1, filter=kp.Filter.eNearest, sampler_address_mode=kp.SamplerAddressMode.eRepeat)
    texture_in_b = mgr.texture(arr_in_b, 3, 1, 1, filter=kp.Filter.eNearest, sampler_address_mode=kp.SamplerAddressMode.eRepeat)
    image_out = mgr.image(arr_out, 3, 1, 1)

    params = [texture_in_a, texture_in_b, image_out]

    (mgr.sequence()
        .record(kp.OpSyncDevice(params))
        .record(kp.OpAlgoDispatch(mgr.algorithm(params, spirv)))
        .record(kp.OpSyncLocal([image_out]))
        .eval())

    assert np.all(image_out.data() == arr_in_a * arr_in_b)

def test_type_unsigned_char():

    shader = """
        #version 450
        layout(set = 0, binding = 0) uniform sampler2D valuesLhs;
        layout(set = 0, binding = 1) uniform sampler2D valuesRhs;
        layout(set = 0, binding = 2, r8ui) uniform image2D imageOutput;
        layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

        void main()
        {
            uint index = gl_GlobalInvocationID.x;
            ivec2 outSize = imageSize(imageOutput);

            // Normalized coordinates for texture()
            vec2 uv = (vec2(index) + 0.5) / vec2(outSize);

            imageStore(imageOutput, ivec2(index, 0), texture(valuesLhs, uv) * texture(valuesRhs, uv));
        }
    """

    spirv = compile_source(shader)

    arr_in_a = np.array([2, 3, 2], dtype=np.uint8)
    arr_in_b = np.array([35, 12, 23], dtype=np.uint8)
    arr_out = np.array([0, 0, 0], dtype=np.uint8)

    mgr = kp.Manager()

    texture_in_a = mgr.texture(arr_in_a, 3, 1, 1, filter=kp.Filter.eNearest, sampler_address_mode=kp.SamplerAddressMode.eRepeat)
    texture_in_b = mgr.texture(arr_in_b, 3, 1, 1, filter=kp.Filter.eNearest, sampler_address_mode=kp.SamplerAddressMode.eRepeat)
    image_out = mgr.image(arr_out, 3, 1, 1)

    params = [texture_in_a, texture_in_b, image_out]

    (mgr.sequence()
        .record(kp.OpSyncDevice(params))
        .record(kp.OpAlgoDispatch(mgr.algorithm(params, spirv)))
        .record(kp.OpSyncLocal([image_out]))
        .eval())

    assert np.all(image_out.data() == arr_in_a * arr_in_b)

def test_texture_numpy_ownership():

    arr_in = np.array([1, 2, 3])

    m = kp.Manager()

    t = m.tensor(arr_in)

    # This should increment refcount for tensor sharedptr
    td = t.data()

    assert td.base.is_init() == True
    assert np.all(td == arr_in)

    del t

    assert td.base.is_init() == True
    assert np.all(td == arr_in)

    m.destroy()

    assert td.base.is_init() == False
