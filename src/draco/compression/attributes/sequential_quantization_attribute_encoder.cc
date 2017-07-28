// Copyright 2016 The Draco Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include "draco/compression/attributes/sequential_quantization_attribute_encoder.h"

#include "draco/core/quantization_utils.h"

namespace draco {

SequentialQuantizationAttributeEncoder::
    SequentialQuantizationAttributeEncoder() {}

bool SequentialQuantizationAttributeEncoder::Initialize(
    PointCloudEncoder *encoder, int attribute_id) {
  if (!SequentialIntegerAttributeEncoder::Initialize(encoder, attribute_id))
    return false;
  // This encoder currently works only for floating point attributes.
  const PointAttribute *const attribute =
      encoder->point_cloud()->attribute(attribute_id);
  if (attribute->data_type() != DT_FLOAT32)
    return false;

  // Initialize AttributeQuantizationTransform.
  const int quantization_bits = encoder->options()->GetAttributeInt(
      attribute_id, "quantization_bits", -1);
  if (quantization_bits < 1)
    return false;
  attribute_quantization_transform_.ComputeParameters(*attribute,
                                                      quantization_bits);
  return true;
}

bool SequentialQuantizationAttributeEncoder::
    EncodeDataNeededByPortableTransform(EncoderBuffer *out_buffer) {
  return attribute_quantization_transform_.EncodeParameters(out_buffer);
}

bool SequentialQuantizationAttributeEncoder::PrepareValues(
    const std::vector<PointIndex> &point_ids, int num_points) {
  SetPortableAttribute(
      attribute_quantization_transform_.GeneratePortableAttribute(
          *(attribute()), point_ids, num_points));
  return true;
}

}  // namespace draco
