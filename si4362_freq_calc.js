const fc_inte = 56;
const fc_frac = 909000;
const fc_xo = 30e6;
const outdiv = 8;

const frac = fc_frac / Math.pow(2, 19);

const rf = ((fc_inte + frac) * (2 * fc_xo)) / outdiv;
console.log(`${(rf / 1e6).toFixed(2)} MHz (${frac})`);
