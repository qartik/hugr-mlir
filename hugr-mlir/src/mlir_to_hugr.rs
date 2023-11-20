use std::collections::HashMap;

use hugr::hugr::hugrmut::HugrMut;

use crate::mlir;
use crate::Result;

pub mod hash;

// trait MlirToHugr {
//     fn go(&self, hugr: &HugrMut)

// }
//

// struct TranslationState {
//     scope: HashMap<Value>

// }

// impl TranslationState {

// }

fn build_container<'a, 'b>(
    _builder: &mut impl hugr::builder::Container,
    _region: impl IntoIterator<Item = melior::ir::OperationRef<'a, 'b>>,
) -> Result<()>
where
    'a: 'b,
{
    Ok(())
}

fn block_ops<'c, 'b>(
    block: &'b melior::ir::Block<'c>,
) -> impl Iterator<Item = melior::ir::OperationRef<'c, 'b>> {
    std::iter::successors(block.first_operation(), |x| {
        unsafe { x.to_ref() }.next_in_block()
    })
}

pub fn mlir_to_hugr(op: &melior::ir::Operation<'_>) -> Result<hugr::Hugr> {
    if let Ok(op1) = TryInto::<&mlir::hugr::ModuleOp>::try_into(op) {
        let mut b = hugr::builder::ModuleBuilder::new();
        build_container(&mut b, block_ops(&op1.body()))?;
    }
    todo!()
}
