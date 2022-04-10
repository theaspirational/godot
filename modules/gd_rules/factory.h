
#ifndef CLIPSFACTORY_H
#define CLIPSFACTORY_H

#include "clips_core/clips.h"
#include "core/variant.h"

  Variant data_object_to_variant(void *, dataObject* );
  Variant data_object_to_variant(void *, dataObject& );

  dataObject* variant_to_data_object(void *, const Variant&, dataObject *, bool);


#endif // CLIPSFACTORY_H
