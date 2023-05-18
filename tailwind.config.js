/** @type {import('tailwindcss').Config} */
module.exports = {
  mode:'jit',
  content: ['./views/*.{ejs,js}',
  '*.{html,js}'],
  theme: {
    screens: {
      sm: '480px',
      md: '768px',
      lg: '976px',
      xl: '1440px'
    },
    extend: {
      colors: {
        greeny:'#14b8a6',
        bluish:'#ccfbf1'
      },
      fontFamily: {
        navbar:"'Inter Tight', sans-serif",
      },
    },
  },
  plugins: [],
}
