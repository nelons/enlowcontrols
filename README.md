# Native UWP Controls - Enlow.Controls library

## Contents

The library currently contains the following controls.

- SimpleDataGrid

### SimpleDataGrid

This is a DataGrid control that will work with native applications. There is a DataGrid control as part of the Windows Community Toolkit but this only works for .NET applications. The control requires columns to be defined (either in XAML or via bindings), and supports column resizing, sorting and single/multiple selection.

This library can be built from the source here, or imported from nuget when available.

Examples:

```
xmlns:enlow="using:Enlow.Controls"
...
<enlow:SimpleDataGrid
    x:Name="GridTest"
    Grid.Row="3"
    Margin="10"
    HorizontalAlignment="Left"
    BorderBrush="{StaticResource SystemBaseMediumColor}"
    BorderThickness="1"
    DividerBrush="Green"
    ItemsSource="{x:Bind Rows, Mode=OneWay}"
    OnSelectionChanged="GridTest_OnSelectionChanged"
    SelectionMode="Multiple"
    SettingsPrefix="App3DataGrid">
    <enlow:SimpleDataGrid.ColumnDefinitions>
        <enlow:SimpleDataGridColumnCollection>
            <enlow:SimpleDataGridColumn
                HorizontalContentAlignment="Left"
                CanHideColumn="False"
                ColumnDefaultWidth="180"
                ColumnMinWidth="120"
                ColumnName="Name"
                OnColumnRowSortComparison="SimpleDataGridColumn_OnColumnRowSortComparison">
                <enlow:SimpleDataGridColumn.CellTemplate>
                    <DataTemplate x:DataType="local:DataRow">
                        <HyperlinkButton Content="{x:Bind Name, Mode=OneWay}" />
                    </DataTemplate>
                </enlow:SimpleDataGridColumn.CellTemplate>
            </enlow:SimpleDataGridColumn>
            <enlow:SimpleDataGridColumn
                BindingProperty="Name"
                ColumnMaxWidth="300"
                ColumnMinWidth="100" />
            <enlow:SimpleDataGridColumn
                HorizontalContentAlignment="Right"
                ColumnDefaultWidth="250"
                ColumnMaxWidth="300"
                ColumnMinWidth="150"
                ColumnNameResourceKey="ValueColumn"
                DataBinding="{Binding Value}" />
            <enlow:SimpleDataGridColumn
                BindingProperty="Column3"
                ColumnMaxWidth="120"
                ColumnMinWidth="80" />
        </enlow:SimpleDataGridColumnCollection>
    </enlow:SimpleDataGrid.ColumnDefinitions>
</enlow:SimpleDataGrid>
```

This is not perfect - it will have bugs and will not be feature complete - I am just starting to dogfood it for my applications. i.e. some properties should be DependencyProperties but aren't (yet).

## License

This library is licensed under the MIT license - https://mit-license.org/.

## Contributions

All contributions are welcome, though maintenance is, as should be expected, at the whim of time given full time employment and family life.