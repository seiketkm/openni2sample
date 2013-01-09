cloneしたままだとdevice.open( openni::ANY_DEVICE )でエラーが発生するので
VisualC++のプロジェクトのプロパティ→構成プロパティ→デバッグ→作業ディレクトリを「$(OutDir)」に変更するとうごくよ。