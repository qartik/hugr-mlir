#include "hugr-rs-bridge/hugr-rs-bridge.h"

#include "mlir/IR/Diagnostics.h"
#include "mlir/CAPI/IR.h"

auto hugr_rs_bridge::parse_hugr_json(mlir::Location loc, llvm::StringRef str) -> mlir::FailureOr<hugr_unique_ptr<Hugr>> {
    auto slice = rust::Slice<const std::uint8_t>(str.bytes_begin(), str.size());
    auto r = detail::parse_hugr_json(slice);
    auto raw = r.result.into_raw();
    if(!raw) {
        return mlir::emitError(loc,"Failed to parse hugr json: ") << r.msg;
    }
    assert(r.msg.length() == 0 && "Successful call means no message");
    return mlir::success(hugr_unique_ptr<Hugr>(raw));
}

auto hugr_rs_bridge::parse_hugr_rmp(mlir::Location loc, llvm::ArrayRef<uint8_t> bytes) -> mlir::FailureOr<hugr_unique_ptr<Hugr>> {
    auto slice = rust::Slice<const std::uint8_t>(&bytes.front(), bytes.size());
    auto r = detail::parse_hugr_rmp(slice);
    auto raw = r.result.into_raw();
    if(!raw) {
        return mlir::emitError(loc,"Failed to parse hugr messagepack: ") << r.msg;
    }
    assert(r.msg.length() == 0 && "Successful call means no message");
    return mlir::success(hugr_unique_ptr<Hugr>(raw));
}

auto hugr_rs_bridge::hugr_to_json(mlir::Location loc, Hugr const& hugr) -> mlir::FailureOr<std::string> {
    auto r = detail::hugr_to_json(hugr);
    if(r.result.empty()) {
        return mlir::emitError(loc,"Failed to serialize hugr to json: ") << r.msg;
    }
    assert(r.msg.length() == 0 && "Successful call means no message");
    return mlir::success(r.result);
}

auto hugr_rs_bridge::hugr_to_rmp(mlir::Location loc, Hugr const& hugr) -> mlir::FailureOr<std::vector<uint8_t>> {
    auto r = detail::hugr_to_rmp(hugr);
    if(!r.result) {
        assert(r.size == 0 && "Failed call means size == 0");
        return mlir::emitError(loc,"Failed to serialize hugr to json: ") << r.msg;
    }
    assert(r.msg.length() == 0 && "Successful call means no message");
    std::vector<uint8_t> result_vec(r.result, r.result + r.size);
    return mlir::success(result_vec);
}

auto hugr_rs_bridge::get_example_hugr() -> hugr_unique_ptr<Hugr> {
    auto r = detail::get_example_hugr();
    assert(r && "No escuse to return fail");
    return hugr_unique_ptr<Hugr>(r);
}


mlir::FailureOr<mlir::OwningOpRef<mlir::Operation*>> hugr_rs_bridge::hugr_to_mlir(mlir::MLIRContext* context, Hugr const& hugr) {
    auto wc = wrap(context);
    auto r = detail::hugr_to_mlir(reinterpret_cast<detail::WrappedContext const&>(wc), hugr);
    auto raw = r.result.into_raw();
    if(!raw) {
        return mlir::emitError(mlir::UnknownLoc::get(context), "Failed to convert hugr to mlir: ") << r.msg;
    }
    auto box = rust::Box<detail::WrappedOperation>::from_raw(raw);
    auto op = unwrap(reinterpret_cast<MlirOperation&>(*box));
    return mlir::success(mlir::OwningOpRef<mlir::Operation*>(op));
}
