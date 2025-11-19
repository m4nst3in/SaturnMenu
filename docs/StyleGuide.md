# Noturnal Loader — Guia de Estilo

## Paleta
- Background: #0E1116
- Surface: #131720
- Card: #171C26
- Accent: #8B6CF0 (gradiente #9D82FF → #7C64F0)
- Success: #32D27F, Error: #E05B5B
- Text Primary: #F2F4F8, Text Secondary: #A7AFC2

## Tipografia
- Fonte: Segoe UI
- Heading: 24 Bold
- Subheading: 14 Regular (Secondary)
- Body: 12 Regular
- Caption: 11 Secondary

## Espaçamento e Raio
- Spacing: 4 / 8 / 12 / 16 / 24
- CornerRadius: 8 / 10 / 12 / 14

## Componentes
- NavButton: pill escuro, ícone MDL2, ativo em roxo
- AccentButton: pill com gradiente roxo
- GhostButton: escuro com borda discreta
- InputTextBox/PasswordBox: escuro, foco com borda roxa
- CardBorder: outline sutil + sombra moderada
- Badge: fundo escuro, borda leve
- Toast: auto-dismiss, severidade com cores

## Interações
- Hover: leve mudança de fundo
- Press: opacidade 0.85
- Focus: borda roxa em inputs
- Navegação: Ctrl+1..4
- Enter: autenticar login
- Esc: fechar modais

## Convenções
- Dicionários em `Themes/` (Colors, Typography, Spacing, Styles)
- Converters em `Converters/`
- Serviços de UI em `Services/`
- Views e ViewModels separados por página/componentes

## Expansão
- Adicionar novos componentes seguindo estilos base
- Centralizar strings para i18n futura
- Preferir ícones MDL2 ou vetoriais
