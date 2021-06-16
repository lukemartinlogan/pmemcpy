@0xf6396937bad15b7c;

struct PrimitiveData {
    union {
        d8 @0 : Int8;
        d16 @1 : Int16;
        d32 @2 : Int32;
        d64 @3 : Int64;
        u8 @4 : UInt8;
        u16 @5 : UInt16;
        u32 @6 : UInt32;
        u64 @7 : UInt64;
        f32 @8 : Float32;
        f64 @9 : Float64;
        d8Arr @10 : List(Int8);
        d16Arr @11 : List(Int16);
        d32Arr @12 : List(Int32);
        d64Arr @13 : List(Int64);
        u8Arr @14 : List(UInt8);
        u16Arr @15 : List(UInt16);
        u32Arr @16 : List(UInt32);
        u64Arr @17 : List(UInt64);
        f32Arr @18 : List(Float32);
        f64Arr @19 : List(Float64);
    }
}
